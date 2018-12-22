#!/bin/tcsh
set CURDIR = `pwd`
set HEADERS = `ls *.h *.inl *.mcr`
foreach header (${HEADERS})
	cp -f ${CURDIR}/$header ../Include
end

cp -f ${SRCROOT}/${PRODUCT_NAME} ../Library/${TARGET_NAME}/lib${PROJECT_NAME}.a
ranlib ../Library/${TARGET_NAME}/lib${PROJECT_NAME}.a
