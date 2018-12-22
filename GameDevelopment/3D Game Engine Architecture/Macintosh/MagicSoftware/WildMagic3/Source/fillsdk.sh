#!/bin/tcsh
# Scan through the directories contained int the current one,
# and copy all of the headers/inlines/macros to the SDK Include.
set DIRS = `ls`
foreach dir (${DIRS})
	if (-d $dir && $dir != ${PROJECT_NAME}.pbproj && $dir != build) then
		echo $dir
		cd $dir
		set CURDIR = `pwd`
		set HEADERS = `ls *.h *.inl *.mcr`
		foreach header (${HEADERS})
			cp -f ${CURDIR}/$header ../../Include
		end
		cd ..
	endif
end

# Copy the top-level SDK header to the Include directory.
cp -f ${SRCROOT}/WildMagic3.h ../Include/.
# Copy the actual library file to the SDK Library.
echo "${PROJECT_NAME} ${TARGET_NAME}"
cp -f ${SRCROOT}/${PRODUCT_NAME} ../Library/${TARGET_NAME}/lib${PROJECT_NAME}.a
ranlib ../Library/${TARGET_NAME}/lib${PROJECT_NAME}.a
echo "Done."
