#!/bin/sh
BLDROOT=/root/src

echo  "WIPE HASHTBL++++++++++++++++++++" 
cd  ${BLDROOT}/dstruct/hashtbl/
rm ./hashtbl
 
echo  "WIPE LIST+++++++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/list/
rm ./list
 
echo  "WIPE TREE+++++++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/tree/
rm ./tree
 
echo  "WIPE HASM+++++++++++++++++++++++"  
cd  ${BLDROOT}/hasm/
rm ./hasm

echo  "WIPE HECVM++++++++++++++++++++++"  
cd  ${BLDROOT}/hecvm/
rm ./hecvm
 
echo  "WIPE RAWBIN+++++++++++++++++++++"  
cd  ${BLDROOT}/rawbin/
rm ./rawbin
 
echo  "WIPE FILEDMP++++++++++++++++++++"  
cd  ${BLDROOT}/util/filedmp/
rm ./filedmp
 
echo  "WIPE NUMFMT+++++++++++++++++++++"  
cd  ${BLDROOT}/util/numfmt/
rm ./numfmt
 
echo  "GO HOME+++++++++++++++++++++++++"  
cd  ${BLDROOT}
