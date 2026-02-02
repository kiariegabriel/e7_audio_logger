# components.cmake

# component ARM::CMSIS:CORE@6.2.0
add_library(ARM_CMSIS_CORE_6_2_0 INTERFACE)
target_include_directories(ARM_CMSIS_CORE_6_2_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/ARM/CMSIS/6.3.0/CMSIS/Core/Include"
)
target_compile_definitions(ARM_CMSIS_CORE_6_2_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_link_libraries(ARM_CMSIS_CORE_6_2_0 INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::CMSIS Driver:I2C@2.0.0
add_library(AlifSemiconductor_CMSIS_Driver_I2C_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source/Driver_I2C.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/i2c.c"
)
target_include_directories(AlifSemiconductor_CMSIS_Driver_I2C_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS/6.3.0/CMSIS/Driver/Include"
)
target_compile_definitions(AlifSemiconductor_CMSIS_Driver_I2C_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_CMSIS_Driver_I2C_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_CMSIS_Driver_I2C_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::CMSIS Driver:SAI@2.0.0
add_library(AlifSemiconductor_CMSIS_Driver_SAI_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source/Driver_I2S.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/i2s.c"
)
target_include_directories(AlifSemiconductor_CMSIS_Driver_SAI_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
  "${CMSIS_PACK_ROOT}/ARM/CMSIS/6.3.0/CMSIS/Driver/Include"
)
target_compile_definitions(AlifSemiconductor_CMSIS_Driver_SAI_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_CMSIS_Driver_SAI_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_CMSIS_Driver_SAI_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::Device:SOC Peripherals:DMA@2.0.0
add_library(AlifSemiconductor_Device_SOC_Peripherals_DMA_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source/Driver_DMA.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/dma_ctrl.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/dma_op.c"
)
target_include_directories(AlifSemiconductor_Device_SOC_Peripherals_DMA_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/M55_HE/RTE/Device/AE722F80F55D5LS_M55_HE"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
)
target_compile_definitions(AlifSemiconductor_Device_SOC_Peripherals_DMA_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_Device_SOC_Peripherals_DMA_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_Device_SOC_Peripherals_DMA_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::Device:SOC Peripherals:PDM@2.0.0
add_library(AlifSemiconductor_Device_SOC_Peripherals_PDM_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source/Driver_PDM_Private.h"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Source/Driver_PDM.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/pdm.c"
)
target_include_directories(AlifSemiconductor_Device_SOC_Peripherals_PDM_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Alif_CMSIS/Include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
)
target_compile_definitions(AlifSemiconductor_Device_SOC_Peripherals_PDM_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_Device_SOC_Peripherals_PDM_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_Device_SOC_Peripherals_PDM_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::Device:SOC Peripherals:PINCONF@2.0.0
add_library(AlifSemiconductor_Device_SOC_Peripherals_PINCONF_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/pinconf.c"
)
target_include_directories(AlifSemiconductor_Device_SOC_Peripherals_PINCONF_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
)
target_compile_definitions(AlifSemiconductor_Device_SOC_Peripherals_PINCONF_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_Device_SOC_Peripherals_PINCONF_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_Device_SOC_Peripherals_PINCONF_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::Device:SOC Peripherals:SDMMC@2.0.0
add_library(AlifSemiconductor_Device_SOC_Peripherals_SDMMC_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/sd.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/sd_host.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/source/sdio.c"
)
target_include_directories(AlifSemiconductor_Device_SOC_Peripherals_SDMMC_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/drivers/include"
)
target_compile_definitions(AlifSemiconductor_Device_SOC_Peripherals_SDMMC_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_Device_SOC_Peripherals_SDMMC_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_Device_SOC_Peripherals_SDMMC_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)

# component AlifSemiconductor::Device:Startup@2.0.0
add_library(AlifSemiconductor_Device_Startup_2_0_0 OBJECT
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/cache.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/mpu.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/pm.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/sau_tcm_ns_setup.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/startup.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/system.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/tgu.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/source/vectors.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/system/source/sys_clocks.c"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/system/source/sys_utils.c"
)
target_include_directories(AlifSemiconductor_Device_Startup_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/M55_HE/RTE/Device/AE722F80F55D5LS_M55_HE"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/core/common/include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/soc/AE722F80F55D5/include"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/soc/AE722F80F55D5/include/rtss_he"
  "${CMSIS_PACK_ROOT}/AlifSemiconductor/Ensemble/2.0.4/Device/system/include"
)
target_compile_definitions(AlifSemiconductor_Device_Startup_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
target_compile_options(AlifSemiconductor_Device_Startup_2_0_0 PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(AlifSemiconductor_Device_Startup_2_0_0 PUBLIC
  ${CONTEXT}_ABSTRACTIONS
)
