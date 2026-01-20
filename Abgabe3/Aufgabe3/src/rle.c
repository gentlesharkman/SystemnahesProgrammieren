#include "../include/rle.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct RLENode {
    uint64_t count;
    struct RLENode* next;
} RLENode;

struct RLE {
    RLENode* head;
    RLENode* tail;
    uint64_t size;
};

static void append_to_rle(RLE* rle, uint64_t count) {
    RLENode* node = malloc(sizeof(RLENode));
    node->count = count;
    node->next = NULL;

    if (rle->tail) {
        rle->tail->next = node;
    } else {
        rle->head = node;
    }

    rle->tail = node;
    rle->size += 1;
}

/**
 * Create a new RLE data structure. The RLE data structure is a linked
 * list of RLENodes. Each RLENode contains a count of the number of
 * consecutive bits that are the same. The RLE data structure is
 * initialized with an entry.
 * @return a pointer to the RLE data structure
 */
RLE* create_rle() {
    RLE* rle = malloc(sizeof(RLE));
    rle->head = NULL;
    rle->tail = NULL;
    rle->size = 0;

    append_to_rle(rle, 0); // Start with a count of 0 bits

    return rle;
}

/**
 * Delete the RLE data structure and all of its nodes. This function
 * should be called when the RLE data structure is no longer needed.
 * @param rle the RLE data structure to delete
 */
void delete_rle(RLE* rle) {
    RLENode* node = rle->head;
    while (node) {
        RLENode* next = node->next;
        free(node);
        node = next;
    }
    free(rle);
}

static bool pop_head_rle(RLE* rle, uint64_t* count) {
    if (!rle->head) {
        return false;
    }

    RLENode* node = rle->head;
    *count = node->count;

    rle->head = node->next;
    if (rle->tail == node) {
        rle->tail = NULL;
    }

    free(node);
    rle->size -= 1;

    return true;
}

static uint64_t get_rle_total_count(RLE* rle) {
    uint64_t total = 0;
    RLENode* node = rle->head;
    while (node) {
        total += node->count;
        node = node->next;
    }
    return total;
}

/**
 * Fill rle counts with the provided data. The data should be treated as
 * binary data, not as a string, so the data is not null-terminated.
 *
 * This function counts the number of consecutive bits that are the
 * same and appends that count to the rle. The first entry of the rle
 * should always be the number of consecutive 0s at the beginning of
 * the data.
 *
 * For example, if the start of data is "00001111", then the rle should contain
 * two entries 4 and 4.
 *
 * If the start of data is "11110000", then the rle should contain three entries,
 * 0, 4, and 4
 * @param rle Will be filled with counts
 * @param data Source data, treated as binary data
 * @param size Size of the source data
 */
void encode_rle(RLE* rle, const char* data, size_t size) {
    uint8_t counting_bit = (rle->size & 1) ^ 1;

    for (size_t i = 0; i < size; i++) {
        for (int8_t j = 7; j >= 0; j--) {
            uint8_t current_bit = (data[i] >> j) & 1;
            if (current_bit == counting_bit) {
                rle->tail->count++;;
            } else {
                append_to_rle(rle, 1);
                counting_bit ^= 1; // Switch between 0 and 1
            }
        }
    }
}

/**
 * Decodes the rle to the appropriate binary data. The returned data
 * should be treated as binary data, not as a string, so the data is
 * not null-terminated.
 * @param rle assumed to be filled with counts
 * @param size will be set by this function and is the size of the returned data
 * @return binary data
 */
char* decode_rle(RLE* rle, size_t* size) {
    uint64_t total_bits = get_rle_total_count(rle);
    *size = (total_bits + 7) >> 3; // Round up to the nearest byte

    char* output = calloc(*size, sizeof(char));
    if (!output) {
        return NULL;
    }

    uint64_t count;
    size_t byte_index = 0;

    uint8_t bit = 0;
    uint8_t bit_index = 7;

    while (byte_index < *size && pop_head_rle(rle, &count)) {
        while (count > 0) {
            output[byte_index] |= bit << bit_index;
            if (bit_index == 0) {
                byte_index++;
                bit_index = 7;
            } else {
                bit_index--;
            }
            count--;
        }
        bit ^= 1; // Switch between 0 and 1
    }

    return output;
}

void print_rle(RLE* rle, uint8_t counts_per_line) {
    RLENode* node = rle->head;
    printf("{\n");
    int counter = 0;
    while (node) {
        printf("  %lu", node->count);
        if (node->next) printf(", "); // print comma only if this isn't the last node
        if (counts_per_line > 0 && ++counter >= counts_per_line) {
            printf("\n");
            counter = 0;
        }

        node = node->next;
    }
    printf(" }");
    printf("\n");
}

/**
 * Versuch der Auslagerung ._." 
uint8_t serializeNode(char* serialResult,  uint8_t* serialByte, uint64_t* secCount, uint64_t count, bool type, int size) {
    if(type == 0) { *serialByte | 1; }
    else { *serialByte & 0; }
    *serialByte << size;
    *serialByte | count;
    *secCount -= count;
    if(isHalfbyte) {
        uint8_t temp = serialByte;
        temp >> 4;
        halfbyte << 4;
        halfbyte | temp;
        *serialResult[serialResultIndex++] = (char) halfbyte;
        serialByte << 4;
        halfbyte = serialByte;
        if(serialByte == 0) {
            isHalfbyte = false;
            serialResult[serialResultIndex++] = (char) halfbyte;
        }
    }
    else  { serialResult[serialResultIndex++] = (char) serialByte; }
    
}
*/

char* serialize_rle(RLE* rle, size_t* size) {
    /**
    Probleme: Wenn der count größer als 64
    -> Lösung: Die nächste Einheit kodiert den Rest
    
Milos tragischer Code, der failte.

    bool type = 0;                  //Which type is encoded by the section, 0 or 1
    uint8_t serialByte;             //
    uint64_t serialResultIndex = 0; //Current index in the serialized string
    uint8_t halfbyte = 0;  //Temporarily saves half a byte of serialization
    int resultSize;
    RLENode* currentNode = rle->head;

    while(currentNode) {
        uint64_t currentCount = currentNode->count;
        while (currentCount > 63) {
            resultSize += 2;
            currentCount -= 63;
        }
        if (currentCount > 3) {
            resultSize += 2;
        }  else if (currentCount > 0) {
            resultSize += 1;
        }
        currentNode = currentNode->next;
    }

    char* serialResult = malloc((resultSize + 1) / 2);

    for(RLENode* currentNode = rle->head; currentNode != NULL ; currentNode = currentNode->next) { //Loops through all the nodes
        uint64_t secCount = currentNode->count; //
        serialByte = 0b00000000;
        
        while(secCount > 0) {
            
            if(secCount >= 64) {
                
                if(type == 1) { serialByte = 0b10000000; } //Set type to first bit
                else { serialByte = 0b00000000; }
                //serialByte << 6;
                serialByte |= 0b01111111;
                secCount -= 63;

                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    //halfbyte << 4;
                    halfbyte |= temp;
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    serialByte << 4;
                    halfbyte = serialByte;
                }
                
                else { serialResult[serialResultIndex++] = (char) serialByte; }
            }

            else if(secCount > 3) {
                if(type == 1) { serialByte = 0b100000000; }
                else { serialByte = 0b00000000; }
                //serialByte << 6;
                serialByte |= 0b01000000;
                serialByte |= secCount;
                
                secCount = 0;

                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    halfbyte << 4;
                    halfbyte |= temp;
                    printf("if halfbyte != 0: %d \n", halfbyte);
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    serialByte << 4;
                    halfbyte = serialByte;
                }
                
                else {
                    printf("else: %d \n", serialByte);
                    serialResult[serialResultIndex++] = (char) serialByte;
                }
                
            }

            else {
                
                if(type == 1) { serialByte = 0b1000; }
                else { serialByte = 0b0000; }
                //serialByte << 2;
                
                serialByte |= secCount;
                printf("serialByte: %d", serialByte);
                secCount = 0;
                
                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    halfbyte << 4;
                    halfbyte |= temp;
                    
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    
                    serialByte << 4;
                    if(serialByte != 0) {
                        halfbyte = serialByte;
                    }
                    else { halfbyte = 0; }
                    
                }
                else {
                    halfbyte = serialByte;
                }
                printf("Result: %c \n", serialResult[serialResultIndex]);
                

            }

            
        }
        type++;
        type = type % 2;
        printf("type: %d \n", type);
    }
    if(halfbyte != 0) {
        serialResult[serialResultIndex++] = (char) halfbyte;
    }
    serialResult[serialResultIndex++] = '\0';
    for(char* temp = serialResult; *temp != '\0'; temp++) {
        printf("rawr%c", *temp);
    }
    
    return serialResult;
*/
    // Calculate size of result to allocate memory
    int  resultSize = 0;
    RLENode* node = rle->head;

    while (node) {
        uint64_t currentCount = node->count;
        while (currentCount > 63) {
            resultSize += 2;
            currentCount -= 63;
        }
        if (currentCount > 3) {
            resultSize += 2;
        }  else if (currentCount > 0) { // Wenn aktuell = 63, dann muss hier > 0 überprüft werden
            resultSize += 1;
        }
        node = node->next;
    }

    char* result = malloc((resultSize + 1) / 2);

	node = rle->head;           // current node in interation
	uint8_t type = 0;           // What is encoded
	uint8_t byte = 0;           // 
	uint8_t halfbyte = 0;
	uint8_t temp_byte;
	uint8_t halfbyte_check = 0;
	uint8_t case_value = 0;
	uint64_t currentCount = 0;  //count of current node or what is left
	uint64_t resultIndex = 0;   

	while (node) {

		currentCount = node->count;
		if (currentCount == 0) {
			type++;
            type = type % 2;
		}

		while (currentCount > 0) {

			if (halfbyte_check == 0) { //Bytegrenze

				if (currentCount > 63) {

					case_value = 63;
					byte = (type == 0) ? 0b01000000 : 0b11000000;
					byte |= case_value;
					result[resultIndex++] = (char) byte;
					currentCount -= 63;

				}

				else if (currentCount > 3) {

					case_value = currentCount;
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= case_value;
					result[resultIndex++] = (char) byte;
					currentCount = 0;
					type++;

				}

				else {

					case_value = currentCount;
					byte = (type % 2 == 0) ? 0b00000000 : 0b00001000;
					byte |= case_value;
					halfbyte |= byte;
					halfbyte <<= 4;
					halfbyte_check = 1;
					currentCount = 0;
					type++;

					}
			} else { //Halfbytegrenze

				if (currentCount > 63) {

					case_value = 63;
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= case_value;
					temp_byte = byte; //BBBB CCCC
					byte >>= 4; // 0000 BBBB
					byte |= halfbyte; // AAAA BBBB
					halfbyte = temp_byte << 4; // CCCC 0000
					result[resultIndex++] = (char) byte;
					currentCount -= 63;

				}
				else if (currentCount > 3) {

					case_value = currentCount;
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= case_value;
					temp_byte = byte;
					byte >>= 4;
					byte |= halfbyte;
					halfbyte = temp_byte << 4;
					result[resultIndex++] = (char) byte;
					currentCount = 0;
					type++;

				}
				else {
					case_value = currentCount;
					byte = (type % 2 == 0) ? 0b00000000 : 0b00001000;
					byte |= case_value;
					byte |= halfbyte;
					result[resultIndex++] = (char) byte;
					currentCount = 0;
					type++;
					halfbyte_check = 0;
				}
			}
		}
		node = node->next;
	}

	if (halfbyte_check == 1) {result[resultIndex++] = (char) halfbyte;}

	//*size = resultIndex;

    return result;

}

void deserialize_rle(RLE *rle, const char *data, size_t size) {
    /**
    RLENode currentRLENode;
    rle->head = &currentRLENode;
    rle->tail = &currentRLENode;
    rle->size = 1;
    int secCount;
    uint64_t count;
    bool type = 0;
 
    for(int* halfbyte = (int) data; halfbyte != '\0'; halfbyte+4) {
        if(*halfbyte & 0){

        }
    }
    */
    size_t index = 0;
	uint64_t mul_byte = 0;
	uint8_t byte = data[0];

	if ((byte & 0b10000000) > 0) {append_to_rle(rle, 0);}

	while (index < size * 8) {

		if (index % 8 == 0) {  //Bytegrenze

			byte = data[index / 8];

			if ((byte & 0b01000000) != 0) { //Modus 1, ganzes Byte

				byte &= 0b00111111; //Maske für Anzahl
				if (byte == 63) {
					mul_byte += 63; //Run
				} else if (mul_byte > 0) {
					append_to_rle(rle, mul_byte + byte);
					mul_byte = 0;
				}
				else {
					append_to_rle(rle, byte);
				}
				index += 8;

			} else { //Modus 0, vorderes Halfbyte
				byte >>= 4;
				byte &= 0b00000011; //Maske für Anzahl
				if (mul_byte > 0) {
					append_to_rle(rle, mul_byte + byte);
					mul_byte = 0;
				} else {
					append_to_rle(rle, byte);
				}
				index+= 4;

			}

		} else { //Halfbytegrenze

 			uint8_t byte = data[index / 8];
			if ((byte & 0x0F) == 0) {break;} //wenn Padding -> ende
			if ((byte & 0b00000100) > 0) { //Modus 1 an der Haldbytegrenze

				uint8_t extended_byte = data[index / 8 + 1]; //vordere Hälfte vom nächsten Byte

				byte &= 0x0F; //Maske für zweite Hälfte des Bytes 0000 AAAA
				byte <<= 4; // AAAA 0000
				extended_byte &= 0xF0; //Maske für erste Hälfte des Bytes BBBB 0000
				extended_byte >>= 4; // 0000 BBBB
				byte |= extended_byte; // AAAA BBBB
				byte &= 0b00111111;

				if (byte == 63) {
					mul_byte += 63;
				}
				else if (mul_byte > 0) {
					append_to_rle(rle, mul_byte + byte);
					mul_byte = 0;
				}
				else {
					append_to_rle(rle, byte);
				}

				index+= 8;

			} else { //Modus 0 bei Halfbytegrenze

				byte &= 0b00000011; //Maske für Anzahl
				if (mul_byte > 0) {
					append_to_rle(rle, mul_byte + byte);
					mul_byte = 0;
				} else {
					append_to_rle(rle, byte);
				}
				index+= 4;

			}
		}
	}

	uint64_t trash = 0;
	pop_head_rle(rle, &trash);
}
