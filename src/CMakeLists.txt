include_directories(.)

add_executable(
  test

  test.cpp
)
target_link_libraries(
  test

  snark
)
target_include_directories(
  test

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)

add_executable(
  test-gadget

  test-gadget.cpp
)
target_link_libraries(
  test-gadget

  snark
)
target_include_directories(
  test-gadget

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)



add_executable(
  model

  model.cpp
)
target_link_libraries(
  model

  snark
)
target_include_directories(
  model

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)



add_executable(
  test-knowledge-preimage

  test-knowledge-preimage.cpp
)
target_link_libraries(
  test-knowledge-preimage

  snark
)
target_include_directories(
  test-knowledge-preimage

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)


add_executable(
  test-sha256

  test-sha256.cpp
)
target_link_libraries(
  test-sha256

  snark
)
target_include_directories(
  test-sha256

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)