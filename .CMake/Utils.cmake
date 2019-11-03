################################################################################
# Init
################################################################################
# Reset ALL_INCLUDES
set(ALL_INCLUDES "" CACHE INTERNAL "" FORCE)

################################################################################
# Interface
################################################################################
function(output_name OUTPUT_NAME)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
endfunction()

function(output_directory TARGET_NAME DIRECTORY_PATH)
	set_target_properties(${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIRECTORY_PATH}")
	set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DIRECTORY_PATH}")
	set_target_properties(${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${DIRECTORY_PATH}")
endfunction()

function(download_extract_archive ARCHIVE_NAME SRC_URL ORG_NAME DEST_NAME)
	if(EXISTS "${PROJECT_SOURCE_DIR}/${ORG_NAME}")
		file(RENAME "${PROJECT_SOURCE_DIR}/${ORG_NAME}" "${PROJECT_SOURCE_DIR}/${DEST_NAME}")
	endif()
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${DEST_NAME}")
		set(DELETE_ZIP OFF)
		if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${ARCHIVE_NAME}")
			message(STATUS "Downloading ${SRC_URL}...")
			file(DOWNLOAD "${SRC_URL}" "${PROJECT_SOURCE_DIR}/${ARCHIVE_NAME}")
			set(DELETE_ZIP ON)
		endif()
		message(STATUS "Extracting ${ARCHIVE_NAME} to ${DEST_NAME}...")
		execute_process(COMMAND 
			${CMAKE_COMMAND} -E tar -xf "${PROJECT_SOURCE_DIR}/${ARCHIVE_NAME}"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_QUIET ERROR_QUIET
		)
		file(RENAME "${PROJECT_SOURCE_DIR}/${ORG_NAME}" "${PROJECT_SOURCE_DIR}/${DEST_NAME}")
		if(DELETE_ZIP)
			file(REMOVE "${PROJECT_SOURCE_DIR}/${ARCHIVE_NAME}")
		endif()
	endif()
endfunction()

function(recursive_include DIRECTORY_PATH)
	set(ALL_INCLUDES "${ALL_INCLUDES};${DIRECTORY_PATH}" CACHE INTERNAL "" FORCE)
	target_include_directories(${PROJECT_NAME} PUBLIC "${ALL_INCLUDES}")
endfunction()

function(ensure_files_present FILES_ROOT FILENAMES)
	set(RELATIVE_PATH "${FILES_ROOT}")
	string(FIND "${FILES_ROOT}" "${ROOT_PATH}" SRC_IDX)
	string(LENGTH "${ROOT_PATH}" LEN)
	if(NOT SRC_IDX EQUAL -1)
		string(SUBSTRING "${FILES_ROOT}" ${LEN} -1 RELATIVE_PATH)
		set(RELATIVE_PATH ".${RELATIVE_PATH}")
	endif()
	foreach(FILENAME ${FILENAMES})
		if(NOT EXISTS "${FILES_ROOT}/${FILENAME}")
			message("${RELATIVE_PATH}/${FILENAME} not found!")
			set(ERROR TRUE)
		endif()
	endforeach()
	if(${ERROR})
		message(FATAL_ERROR "Required dependencies missing from ${FILES_ROOT}!")
	endif()
endfunction()

function(configure_files FILES FROM_PREFIX FROM_SUFFIX TO_PREFIX TO_SUFFIX PARAM)
	foreach(FILENAME ${FILES})
		configure_file(
			"${FROM_PREFIX}${FILENAME}${FROM_SUFFIX}"
			"${TO_PREFIX}${FILENAME}${TO_SUFFIX}"
			${PARAM}
		)
	endforeach()
endfunction()

function(configure_files_src_to_bin FILES)
	configure_files("${FILES}" "${CMAKE_CURRENT_SOURCE_DIR}/" ".in" "${CMAKE_CURRENT_BINARY_DIR}/" "" "")
endfunction()

function(copy_files_src_to_bin FILES)
	configure_files("${FILES}" "${CMAKE_CURRENT_SOURCE_DIR}/" "" "${CMAKE_CURRENT_BINARY_DIR}/" "" COPYONLY)
endfunction()

function(install_file_list ALL_FILES SOURCE DEST)
	list(TRANSFORM ALL_FILES PREPEND "${SOURCE}/")
	install(FILES ${ALL_FILES} DESTINATION "${DEST}")
endfunction()

function(install_directory_list ALL_FILES SOURCE DEST)
	list(TRANSFORM ALL_FILES PREPEND "${SOURCE}/")
	install(DIRECTORY ${ALL_FILES} DESTINATION "${DEST}")
endfunction()

function(copy_file_list ALL_FILES SOURCE DEST)
	list(TRANSFORM ALL_FILES PREPEND "${SOURCE}/")
	file(COPY ${ALL_FILES} DESTINATION "${DEST}")
endfunction()
