# 10.3.5 Alternate Functions of Port F
The Port F has an alternate function as analog input for the ADC as shown in Table 10-13 on page 82. If some
Port F pins are configured as outputs, **it is essential that these do not switch when a conversion is in progress**.
This might corrupt the result of the conversion.