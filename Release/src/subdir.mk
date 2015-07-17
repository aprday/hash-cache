################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cache-hash-table-p.cpp \
../src/cache-hash-table.cpp \
../src/standard-hash-table.cpp \
../src/test001.cpp 

OBJS += \
./src/cache-hash-table-p.o \
./src/cache-hash-table.o \
./src/standard-hash-table.o \
./src/test001.o 

CPP_DEPS += \
./src/cache-hash-table-p.d \
./src/cache-hash-table.d \
./src/standard-hash-table.d \
./src/test001.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -msse4.2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


