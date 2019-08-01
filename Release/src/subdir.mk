################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/arrays.c \
../src/buffered_io.c \
../src/dict.c \
../src/example-archiver.c \
../src/hash.c \
../src/lzw_edu.c \
../src/tst.c 

OBJS += \
./src/arrays.o \
./src/buffered_io.o \
./src/dict.o \
./src/example-archiver.o \
./src/hash.o \
./src/lzw_edu.o \
./src/tst.o 

C_DEPS += \
./src/arrays.d \
./src/buffered_io.d \
./src/dict.d \
./src/example-archiver.d \
./src/hash.d \
./src/lzw_edu.d \
./src/tst.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DNDEBUG=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


