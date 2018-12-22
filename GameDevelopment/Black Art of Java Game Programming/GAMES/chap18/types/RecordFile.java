package types;

import java.io.*;
import java.util.BitSet;

// File format:  Block_Size, First_Free, [block] ...
// Block format: next_block, [data]

public class RecordFile {
  private final int DEFAULT_BLOCK_SIZE = 128;
  
  RandomAccessFile F;
  int Block_Size;
  int Actual_Size;
  int First_Free;
  int Total_Blocks;
  byte[] Read_Buffer;
  ByteArrayOutputStream Block_Buffer = new ByteArrayOutputStream();
  
  public RecordFile (String f_name) throws IOException {
    F = new RandomAccessFile(f_name, "rw");
    int len = (int)F.length();
    if (len == 0) {
      F.writeInt(Block_Size = DEFAULT_BLOCK_SIZE);
      F.writeInt(First_Free = -1);
    } else {
      Block_Size = F.readInt();
      First_Free = F.readInt();
      if ((len - 8) % Block_Size != 0)
	throw new IOException("RecordFile: file not aligned");
      Total_Blocks = (len - 8) / Block_Size;
    }
    Actual_Size = Block_Size - 4;
    Read_Buffer = new byte[Actual_Size];
  }
  public synchronized int numBlocks () {
    return Total_Blocks;
  }
  public synchronized byte[] readAll (int block) throws IOException {
//    System.out.println(">>readAll: " + block);
    Block_Buffer.reset();
    do {
      F.seek(8L + Block_Size * block);
      block = F.readInt();
      F.readFully(Read_Buffer);
      Block_Buffer.write(Read_Buffer);
    } while (block != -1);
//    System.out.println("<<readAll: " + block);
    return Block_Buffer.toByteArray();
  }
  public synchronized byte[] readPartial (int block, int off, int len) throws IOException {
//    System.out.println(">>readPartial: " + block);
    Block_Buffer.reset();
    do {
      F.seek(8L + Block_Size *block);
      block = F.readInt();
      if (off < Actual_Size) {
	F.skipBytes(off);
	int this_len = len > Actual_Size - off ? Actual_Size - off : len;
	F.readFully(Read_Buffer, 0, this_len);
	Block_Buffer.write(Read_Buffer, 0, this_len);
	len -= this_len;
	off = 0;
      } else {
	off -= Actual_Size;
      }
    } while (len != 0 && block != -1);
//    System.out.println("<<readPartial: " + block);
    if (len == 0)
      return Block_Buffer.toByteArray();
    throw new EOFException("RecordFile.readPartial(): end of record");
  }
  public synchronized void seekBlock (int block) throws IOException {
    F.seek(12L + Block_Size * block);
  }
  private int getFree () throws IOException {
    if (First_Free == -1) {
      F.seek(8L + Block_Size * (Total_Blocks + 1) - 1);
      F.writeByte((byte)0);
      return Total_Blocks++;
    } else {
      int free = First_Free;
      F.seek(8L + Block_Size * First_Free);
      First_Free = F.readInt();
      return free;
    }
  }
  private void setFree (int block) throws IOException {
    F.seek(8L + Block_Size * block);
    F.writeInt(First_Free);
    First_Free = block;
  }
  private void syncFree () throws IOException {
    F.seek(4L);
    F.writeInt(First_Free);
  }
  public synchronized int newBlock () throws IOException {
    int block = getFree();
    F.seek(8L + Block_Size * block);
    F.writeInt(-1);
    syncFree();
    return block;
  }
  public synchronized void writeAll (int block, byte[] data) throws IOException {
    boolean old_blocks = true;
    int len = data.length;
    int off = 0;
    int n_block = 0;
//    System.out.println(">>writeAll: " + block);
    while (true) {
      if (old_blocks) {
	F.seek(8L + Block_Size * block);
	n_block = F.readInt();
      } else {
	F.seek(12L + Block_Size * block);
	n_block = -1;
      }
      F.write(data, off, len > Actual_Size ? Actual_Size : len);
      if (len > Actual_Size) {
	len -= Actual_Size;
	off += Actual_Size;
	if (n_block == -1) {
	  old_blocks = false;
	  n_block = getFree();
	  F.seek(8L + Block_Size * block);
	  F.writeInt(n_block);
	}
	block = n_block;
      } else {
	F.seek(8L + Block_Size * block);
	F.writeInt(-1);
	while ((block = n_block) != -1) {
	  F.seek(8L + Block_Size * block);
	  n_block = F.readInt();
	  setFree(block);
	}
	syncFree();
//	System.out.println("<<writeAll: " + block);
	return;
      }
    }
  }
  public synchronized void writePartial (int block, byte[] data, int off, int len) throws IOException {
//    System.out.println(">>writePartial: " + block);
    boolean old_blocks = true;
    int n_block = 0;
    if (len > data.length)
      len = data.length;
    int start = 0;
    do {
      if (old_blocks) {
	F.seek(8L + Block_Size * block);
	n_block = F.readInt();
      } else {
	F.seek(12L + Block_Size * block);
	n_block = -1;
      }
      if (off < Actual_Size) {
	F.skipBytes(off);
	int this_len = len > Actual_Size - off ? Actual_Size - off : len;
	F.write(data, start, this_len);
	start += this_len;
	len -= this_len;
	off = 0;
      } else {
	off -= Actual_Size;
      }
      if (len > 0 && n_block == -1) {
	old_blocks = false;
	n_block = getFree();
	F.seek(8L + Block_Size * block);
	F.writeInt(n_block);
      }
      block = n_block;
    } while (len > 0);
    syncFree();
//    System.out.println("<<writePartial: " + block);
  }
  public synchronized BitSet scanBlocks () throws IOException {
    BitSet bs = new BitSet(Total_Blocks);
    int n = First_Free;
    while (n != -1) {
      bs.set(n);
//      System.out.println("Free block " + n);
      F.seek((long)8 + Block_Size * n);
      n = F.readInt();
    }
    for (int i = 0; i < Total_Blocks; i++) {
      if (!bs.get(i)) {  // bit clear means header block
	n = i;
//	System.out.println("First block!");
	do {
//	  System.out.println("Scanning block " + n);
	  F.seek((long)8 + Block_Size * n);
	  n = F.readInt();
	  if (n != -1)
	    bs.set(n);
	} while (n != -1);
      }
    }
//    System.out.println("Done scanning");
    return bs;
  }
}
	
