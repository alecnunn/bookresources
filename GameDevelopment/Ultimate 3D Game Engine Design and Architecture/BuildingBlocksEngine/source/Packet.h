/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PACKET_H_
#define _BB_PACKET_H_

#include<string.h>
#include<Socket.h>
#include<NameSpace.h>
#include<Endian.h>


DECLARE_ENGINE_NAMESPACE


class Packet
{
   public:
      Packet(BPACKETVALUE usage, BPACKETVALUE id, const char *data, int size)
      {
         m_usage = usage;
         m_id = id;
         m_dataSize = 0;
         m_data[0] = '\0';

         m_dataSize = size;

         if(data != NULL)
            memcpy(m_data, data, m_dataSize);

         Endian endian;
         endian.ResolveEndian((char*)&m_dataSize,
                              sizeof(m_dataSize),
                              BB_ENDIAN_LITTLE);
      }

      ~Packet()
      {

      }

      int GetPacketSize() { return sizeof(*this); }
      BPACKETVALUE GetUsage() { return m_usage; }
      BPACKETVALUE GetID() { return m_id; }
      BPACKETVALUE *GetData() { return m_data; }

      int GetDataSize()
      {
         Endian endian;
         int temp = m_dataSize;

         endian.ResolveEndian((char*)&temp, sizeof(temp),
                              BB_ENDIAN_LITTLE);

         return temp;
      }

   private:
      BPACKETVALUE m_usage;
      BPACKETVALUE m_id;
      int m_dataSize;
      BPACKETVALUE m_data[BB_MAX_PACKET_SIZE];
};

END_ENGINE_NAMESPACE

#endif
