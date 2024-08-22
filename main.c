#include "tm4c123gh6pm.h"
#include <stdint.h>

void UART0_Init(void);
void UART0_Handler(void);
void UART0_WriteChar(char c);
char UART0_ReadChar(void);

// Initialize UART0
void UART0_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01;   // Enable UART0 clock
    SYSCTL_RCGCGPIO_R |= 0x01;   // Enable Port A clock

    UART0_CTL_R &= ~0x01;        // Disable UART0 during setup
    UART0_IBRD_R = 104;          // 16MHz / (16 * 9600) = 104.166 (integer part)
    UART0_FBRD_R = 11;           // Fractional part (0.166 * 64) + 0.5 = 11
    UART0_LCRH_R = 0x60;         // 8-bit, no parity, 1-stop bit
    UART0_CC_R = 0x00;           // Use system clock
    UART0_CTL_R = 0x301;         // Enable UART0, TXE, RXE

    GPIO_PORTA_AFSEL_R |= 0x03;  // Enable alt function on PA1-0
    GPIO_PORTA_PCTL_R |= 0x11;   // Configure PA1-0 as UART
    GPIO_PORTA_DEN_R |= 0x03;    // Enable digital on PA1-0

    UART0_IM_R |= 0x10;          // Enable RX interrupt
    NVIC_EN0_R |= 1 << 5;        // Enable interrupt 5 in NVIC
}

// UART0 interrupt handler
void UART0_Handler(void) {
    char c;
    if (UART0_MIS_R & 0x10) {    // Check if interrupt caused by RX
        UART0_ICR_R = 0x10;      // Clear the interrupt flag
        c = UART0_ReadChar();    // Read the received character
        
        // Convert to uppercase if it's a lowercase letter
        if (c >= 'a' && c <= 'z') {
            c = c - 32;          // Convert to uppercase
        } else {
            c = c + 1;           // Send the next character
        }
        
        UART0_WriteChar(c);      // Send the modified character back
    }
}

// Function to send a character over UART
void UART0_WriteChar(char c) {
    while ((UART0_FR_R & 0x20) != 0);  // Wait until TXFF is 0 (TX not full)
    UART0_DR_R = c;
}

// Function to receive a character from UART
char UART0_ReadChar(void) {
    while ((UART0_FR_R & 0x10) != 0);  // Wait until RXFE is 0 (RX not empty)
    return (char)(UART0_DR_R & 0xFF);
}

int main(void) {
    UART0_Init();                // Initialize UART0
    __enable_irq();              // Enable global interrupts

    while (1) {
        // Main loop can perform other tasks or enter a low-power mode
    }
}
