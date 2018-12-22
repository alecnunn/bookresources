#!/bin/sh
BLDROOT=/root/src

echo  "MOV HASHTBL+++++++++++++++++++++" 
cd  ${BLDROOT}/dstruct/hashtbl/
cp ./hashtbl  ${BLDROOT}/bin/hashtbl
 
echo  "MOV LIST++++++++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/list/
cp ./list  ${BLDROOT}/bin/list
 
echo  "MOV TREE++++++++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/tree/
cp ./tree  ${BLDROOT}/bin/tree
 
echo  "MOV HASM++++++++++++++++++++++++"  
cd  ${BLDROOT}/hasm/
cp ./hasm  ${BLDROOT}/bin/hasm

echo  "MOV HECVM+++++++++++++++++++++++"  
cd  ${BLDROOT}/hecvm/
cp ./hecvm  ${BLDROOT}/bin/hecvm
 
echo  "MOV RAWBIN++++++++++++++++++++++"  
cd  ${BLDROOT}/rawbin/
cp ./rawbin  ${BLDROOT}/bin/rawbin
 
echo  "MOV FILEDMP+++++++++++++++++++++"  
cd  ${BLDROOT}/util/filedmp/
cp ./filedmp  ${BLDROOT}/bin/filedmp
 
echo  "MOV NUMFMT++++++++++++++++++++++"  
cd  ${BLDROOT}/util/numfmt/
cp ./numfmt  ${BLDROOT}/bin/numfmt

echo  "GO HOME+++++++++++++++++++++++++"  
cd  ${BLDROOT}
