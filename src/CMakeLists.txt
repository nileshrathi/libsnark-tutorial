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
  model2

  model2.cpp
)
target_link_libraries(
  model2

  snark
  gtest
)
target_include_directories(
  model2

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/gtest
)




add_executable(
  model3

  model3.cpp
)
target_link_libraries(
  model3

  snark
  gtest
)
target_include_directories(
  model3

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/gtest
)




add_executable(
  verifier

  verifier.cpp
)
target_link_libraries(
  verifier

  snark
  gtest
)
target_include_directories(
  verifier

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/gtest
)



add_executable(
  receiver

  receiver.cpp
)
target_link_libraries(
  receiver

  snark
  gtest
)
target_include_directories(
  receiver

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/gtest
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







add_executable(
  temp

  temp.cpp
)
target_link_libraries(
  temp

  snark
  gtest
)
target_include_directories(
  temp

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/gtest
)