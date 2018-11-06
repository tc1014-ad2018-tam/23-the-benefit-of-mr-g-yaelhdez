//Carlos Yael Hernandez Alcala
//A01412016
//A01412016@ITESM.MX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_ROWS 400
#define MAX_COLS 200
#define MAX_SEATS MAX_ROWS * MAX_COLS
#define BUFFER_SIZE 300
#define FILE_NAME "registry.txt"

// Base struct for every ticket
typedef struct Ticket {
    char name[50];   // Name of the buyer
    int row;         // Row number (1 - 400)
    int seat;        // Seat number (1 - 200)
    int paymentType; // 1 - Visa, 2 - MasterCard, 3 - American Express
    float price;     // Ticket total price
} Ticket_t;

// Array of sales as "history"
Ticket_t SalesHistory[MAX_SEATS];
// Stores the length (size) of SalesHistory array
// in order to help with iterations.
int sales_length = 0;

// Function definitions
void wait();
void cleanBuffer();
void addMargin(int size);
int validateInput(int value, int max, const char *property);
char **strsplit(char *str, const char *delimiter);

// Main options
void salesProcess();
void saveAllData();
void computeReports();

// Sales functions
int getSeat(int min, int max, const char *type, const char *range);
int seatIsTaken(Ticket_t *ticket);

// Report functions
float getTotalSales();
float getOccupationRate();

// File management
void fileRead();

int main() {
    int main_option;

    // Load all previous tickets
    fileRead();

    do {
        // Prints main options
        printf("Write the number of the option you want to execute.\n");
        printf("1 - Sell a ticket.\n");
        printf("2 - Save all data.\n");
        printf("3 - Get reports.\n");
        printf("4 - Exit.\n");
        printf("\nYour choice is: ");
        scanf("%i", &main_option);
        cleanBuffer();

        // Validate the input value
        if (!validateInput(main_option, 4, "option")) {
            addMargin(1); // Add padding
            continue; // Skip the rest of the code if the input is invalid
        }

        switch (main_option) {
            case 1: salesProcess(); break;
            case 2: saveAllData(); break;
            case 3: computeReports(); break;
        }

        // Add more padding
        addMargin(1);

        // Only wait for a key press if the selected opcion isn't 4 (exit).
        // There is no need to wait for a key if the user requested for the exit.
        if (main_option != 4) {
            wait();
            addMargin(2); // Add more more margin
        }
    } while (main_option != 4);

    printf("Program finished.\n");

    // Do you remember the compiler warnings?
    return 0;
}

// Prints a nice message telling the user that the program is
// waiting for a key to be pressed in order to continue with
// its execution.
//
// In other words, system("pause");
void wait() {
    printf("Press any key to continue.\n");
    getchar();
}

// Clears the input buffer in order to prevent invalid
// characters to come in after a scanf instruction.
void cleanBuffer() {
    int buffer;
    while ((buffer = getchar()) != EOF && buffer != '\n');
}

// Generates line breaks in order to
// add padding betweeen lines.
void addMargin(int size) {
    for (int i = 0; i < size; i++) {
        printf("\n");
    }
}

// Validate user input with the min an max values
int validateInput(int value, int max, const char *property) {
    if (value < 1 || value > max) {
        printf("\nInvalid %s. Please, try again.\n", property);
        return 0;
    }

    return 1;
}

// Splits a string using a given character (delimiter)
char **strsplit(char *str, const char *delimiter) {
    char **result = 0; // Splitted string
    size_t count = 0;  // Size of the string
    char *tmp = str;   // Temporary copy of the original string
    char *last_delim = 0;
    char delim[2];

    delim[0] = delimiter;
    delim[1] = 0;

    // Count how many elements will be extracted
    while (*tmp) {
        if (delimiter == *tmp) {
            count++;
            last_delim = tmp;
        }

        tmp++;
    }

    // Add space for trailing token
    count += last_delim < (str + strlen(str) - 1);

    // Add space for terminating null string, so caller
    // knows where the list of returned string ends.
    count++;
    result = malloc(sizeof(char*) * count); // Get memory for the new strings

    if (result) {
        size_t idx = 0;
        char *token = strtok(str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }

        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void salesProcess() {
    // Create new "fly" ticket
    Ticket_t ticket;

    addMargin(2); // Add two lines of padding

    // Get guest name.
    printf("Write your name.\n");

    // Stores the input data on the ticket struct
    scanf("%s", &ticket.name);

    // Controls if the current selection of row & seat were
    // taken by another passenger.
    int isTaken;

    do {
        do {
            // Get seat's row number
            printf("\nRow number (1 - %i): ", MAX_ROWS);
            scanf("%i", &ticket.row);
            // Every time we use scanf for int's,
            // it is necessary to call a clearBuffer to avoid console errors
            cleanBuffer();

            // Validate the input value
            if (validateInput(ticket.row, MAX_ROWS, "row")) {
                // If the input value is valid,
                // then we can exit this do-while loop
                // and continue with the next step.
                // Otherwise, if the value is off-limits,
                // this loop will continue asking the user to enter
                // the correct data.
                break;
            }
        } while (1);

        char message[10];
        sprintf(message, "1 - %i", MAX_COLS);

        // Get the remaining data
        ticket.seat = getSeat(1, MAX_COLS, "seat", message);
        ticket.price = 200.f; // Coming soon

        // Repeat the whole process if the selected seat & row are taken.
        isTaken = seatIsTaken(&ticket);
    } while (isTaken == 1);

    do {
        printf("\nChoose a payment method:\n");
        printf("1 - Visa.\n");
        printf("2 - MasterdCard.\n");
        printf("3 - American Express.\n");
        printf("\nYour choice is: ");
        scanf("%i", &ticket.paymentType);
        cleanBuffer();

        if (validateInput(ticket.paymentType, 3, "payment method")) {
            break;
        }
    } while (1);

    // Save this ticket in SalesHistory
    SalesHistory[sales_length++] = ticket;
}

// Saves all data stored on SalesHistory
void saveAllData() {
    // Open the registry file
    FILE *file = fopen(FILE_NAME, "w");

    // Iterate each ticket
    for (int i = 0; i < sales_length; i++) {
        Ticket_t ticket = SalesHistory[i];

        // The buffer that will be write on the file
        char buffer[BUFFER_SIZE];

        // A serialized struct looks like this:
        //
        // Format:
        //  name:row:seat:paymentMethod:price;
        //
        // Example:
        //  James Dean:1:3:1:1200.500;
        snprintf(buffer, BUFFER_SIZE, "%s:%i:%i:%i:%.3f;", ticket.name, ticket.row, ticket.seat, ticket.paymentType, ticket.price);
        fwrite(buffer, 1, strlen(buffer), file);
    }

    fclose(file);
    addMargin(2);
    printf("The data has been saved successfully.");
    addMargin(2);
}

// Initialize all the report system
void computeReports() {
    int option;

    addMargin(2);
    printf("Select an option.\n");
    printf("1 - Total tickets sold.\n");
    printf("2 - Total income.\n");
    printf("3 - Ocuppation rate.\n");

    do {
        printf("\nYour choice is: ");
        scanf("%i", &option);
        cleanBuffer();

        if (validateInput(option, 3, "option selected")) {
            break;
        }
    } while (1);

    float result;
    switch (option) {
        case 1:
            // sales_length stores all selled tickets.
            printf("\nTotal tickets: %i", sales_length);
            break;
        case 2:
            result = getTotalSales();
            printf("\nTotal income: $%.3f", result);
            break;
        case 3:
            result = getOccupationRate();
            printf("\nOccupation rate: %.3f", result);
            break;
    }
}

// Ask the user to enter the correct value of seat
int getSeat(int min, int max, const char *type, const char *range) {
    int seat;

    do {
        printf("\nSeat number (%s): ", range);
        scanf("%i", &seat);
        cleanBuffer();

        if (seat < min || seat > max) {
            printf("\nOnly seats from %i to %i are available for %s side. Please, try again.\n", min, max, type);
        }
        else {
            break;
        }
    } while (1);

    return seat;
}

// Checks if the selected seat is already taken by someone else
int seatIsTaken(Ticket_t *ticket) {
    // Iterates through sales history
    for (int i = 0; i < sales_length; i++) {
        // Get the sale ticket
        Ticket_t sale = SalesHistory[i];

        if (sale.row == ticket->row && sale.seat == ticket->seat) {
            printf("\nThis seat was already taken by %s.", sale.name);
            addMargin(2);
            return 1; // Seat was taken
        }
    }

    return 0; // Seat is empty
}

// Calculates the total earnings for all the tickets selled
float getTotalSales() {
    float total = 0.f;

    for (int i = 0; i < sales_length; i++) {
        Ticket_t ticket = SalesHistory[i];
        total += ticket.price;
    }

    return total;
}

// Calculates the occupation rate
float getOccupationRate() {
    return sales_length / (float) MAX_SEATS;
}

// Reads the registry file and parse its contents
// and restore the previus tickets.
void fileRead() {
    // Open the file in read mode
    FILE *file = fopen(FILE_NAME, "r");

    // If the file doesn't exists (is null), then skip the whole process
    // in order to prevent any program crash
    if (file != NULL) {
        char *buffer; // Will store file's content
        long length;  // Size in bytes of the file

        // Read the file till the end
        fseek(file, 0, SEEK_END);
        // Get the file size
        length = ftell(file);

        // Go back to the beggining
        fseek(file, 0, SEEK_SET);

        // If the file has content...
        if (length > 0) {
            // Reserve some memory
            buffer = malloc(length);

            // Read all and store it on buffer.
            fread(buffer, 1, length, file);

            // Now, we will parse the content in order
            // to regenerate previous tickets.
            //
            // So first of all, each serialized string is
            // separated by a semicolon.
            // So, we will split the content by each semicolon.
            char **tokens = strsplit(buffer, ';');

            // If there's at least one semicolon...
            if (tokens) {
                // Iterate through each serialized string
                for (int i = 0; *(tokens + i); i++) {
                    char *tok = *(tokens + i);
                    Ticket_t ticket;

                    // A serialized struct looks like this:
                    //
                    // Format:
                    //  name:row:seat:paymentMethod:price;
                    //
                    // Example:
                    //  James Dean:1:3:1:1200.500;
                    sscanf(tok, "%[^:]:%i:%i:%i:%f", &ticket.name, &ticket.row, &ticket.seat, &ticket.paymentType, &ticket.price);

                    // Save this ticket in SalesHistory
                    SalesHistory[sales_length++] = ticket;
                    // Free some memory...
                    free(&tok);
                }
            }

            // Free more memory
            free(tokens);
            free(buffer);
        }
    }

    // Close the file
    fclose(file);
}