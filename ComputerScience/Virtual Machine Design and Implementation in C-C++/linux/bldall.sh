#!/bin/sh
BLDROOT=/root/src
echo Build root is $BLDROOT

echo " "
echo  "BUILDING HASHTBL++++++++++++++++" 
cd  ${BLDROOT}/dstruct/hashtbl/
rm ./hashtbl
make hashtbl

echo " " 
echo  "BUILDING LIST+++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/list/
rm ./list
make list

echo " " 
echo  "BUILDING TREE+++++++++++++++++++"  
cd  ${BLDROOT}/dstruct/tree/
rm ./tree
make tree

echo " " 
echo  "BUILDING HASM+++++++++++++++++++"  
cd  ${BLDROOT}/hasm/
rm ./hasm
make hasm

echo " " 
echo  "BUILDING HECVM++++++++++++++++++"  
cd  ${BLDROOT}/hecvm/
rm ./hecvm
make hecvm

echo " " 
echo  "BUILDING RAWBIN+++++++++++++++++"  
cd  ${BLDROOT}/rawbin/
rm ./rawbin
make rawbin

echo " " 
echo  "BUILDING FILEDMP++++++++++++++++"  
cd  ${BLDROOT}/util/filedmp/
rm ./filedmp
make filedmp

echo " " 
echo  "BUILDING NUMFMT+++++++++++++++++"  
cd  ${BLDROOT}/util/numfmt/
rm ./numfmt
make numfmt

echo " " 
echo  "GO HOME+++++++++++++++++++++++++"  
cd  ${BLDROOT}
