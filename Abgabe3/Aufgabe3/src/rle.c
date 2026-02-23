#include "../include/rle.h"
#include <stdlib.h>
#include <stdio.h>

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

char* serialize_rle(RLE *rle, size_t* size) {
	/**
    Probleme: Wenn der count größer als 64
    -> Lösung: Die nächste Einheit kodiert den Rest
	*/

	// Calculate size of result string to alocate memory
    int  resultSize = 0;
    RLENode* node = rle->head;

    while (node) {
        uint64_t count = node->count;
        while (count > 63) {
            resultSize += 2;
            count -= 63;
        }
        if (count > 3) {
            resultSize += 2;
        }  else if (count > 0) { 
            resultSize += 1;
        }
        node = node->next;
    }
    char *result = malloc((resultSize + 1) / 2);

	node = rle->head;			// current node of iteration through RLE
	int type = 0;				// Type that is encoded by current section
	uint8_t byte = 0;			// Current to be serialized byte
	uint8_t halfbyte = 0;		// Saves halfbytes so that only full bytes are added to result
	uint8_t temp_byte;
	uint8_t halfbyte_check = 0;
	uint64_t count = 0;			// Current count to be serialized
	uint64_t index = 0;			// Index of result string

	while (node) {

		count = node->count;
		if (count == 0) {
			type++;
		}
		while (count > 0) {

			if (halfbyte_check == 0) { //Bytegrenze

				if (count > 63) {
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= 63;
					result[index++] = (char) byte;
					count -= 63;

				}

				else if (count > 3) {
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= count;
					result[index++] = (char) byte;
					count = 0;
					type++;

				}

				else {
					byte = (type % 2 == 0) ? 0b00000000 : 0b00001000;
					byte |= count;
					halfbyte |= byte;
					halfbyte <<= 4;
					halfbyte_check = 1;
					count = 0;
					type++;

				}
			}
			
			else { // 

				if (count > 63) {
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= 63;
					temp_byte = byte; 
					byte >>= 4; 
					byte |= halfbyte; 
					halfbyte = temp_byte << 4;
					result[index++] = (char) byte;
					count -= 63;

				}
				else if (count > 3) {
					byte = (type % 2 == 0) ? 0b01000000 : 0b11000000;
					byte |= count;
					temp_byte = byte;
					byte >>= 4;
					byte |= halfbyte;
					halfbyte = temp_byte << 4;
					result[index++] = (char) byte;
					count = 0;
					type++;

				}
				else {
					byte = (type % 2 == 0) ? 0b00000000 : 0b00001000;
					byte |= count;
					byte |= halfbyte;
					result[index++] = (char) byte;
					count = 0;
					type++;
					halfbyte_check = 0;
				}
			}
		}
		node = node->next;
	}

	if (halfbyte_check == 1) {result[index++] = (char) halfbyte;}

	*size = index;

    return result;
}




void deserialize_rle(RLE *rle, const char *data, size_t size) {
	size_t index = 0;		// 
	uint64_t tempCount = 0; // Temp count incase more than 2 sections for one node
	uint8_t byte = data[0]; // Iteration through the bytes in result string
	uint8_t currentType;
	uint8_t nextType;

	if ((byte & 0b10000000) > 0) {append_to_rle(rle, 0);} // If the first encoded type is 1

	while (index < size * 8) {

		if (index % 8 == 0) {  //

			byte = data[index / 8];

			if ((byte & 0b01000000) != 0) { // 2 sections
				uint8_t nextByte = data[(index / 8) + 1];
				currentType = byte & 0b10000000;
				nextType = nextByte & 0b10000000;
				byte &= 0b00111111; //
				if (byte == 63) {
					if(currentType == nextType) {
						tempCount += 63;
					}
					else { append_to_rle(rle, byte); }

				}
				else if (tempCount > 0) {
					append_to_rle(rle, tempCount + byte);
					tempCount = 0;
				}
				else {
					append_to_rle(rle, byte);
				}
				index += 8;

			}
			else { // 1 section
				byte >>= 4;
				byte &= 0b00000011; //Maske for count
				if (tempCount > 0) {
					append_to_rle(rle, tempCount + byte);
					tempCount = 0;
				} else {
					append_to_rle(rle, byte);
				}
				index+= 4;

			}

		}
		
		else {

 			uint8_t byte = data[index / 8];
			if ((byte & 0x0F) == 0) {break;} // Padding at the end due to halfbyte
			if ((byte & 0b00000100) > 0) { // 2 sections

				uint8_t extended_byte = data[index / 8 + 1]; 

				// stitch byte together
				byte &= 0x0F; 
				byte <<= 4;
				nextType = (extended_byte & 0b00001000) << 4;
				extended_byte &= 0xF0; 
				extended_byte >>= 4; 
				byte |= extended_byte; 
				byte &= 0b00111111;

				
				currentType = byte & 0b10000000;
				byte &= 0b00111111; //
				if (byte == 63) {
					if(currentType == nextType) {
						tempCount += 63;
					}
					else { append_to_rle(rle, byte); }
				}
				else if (tempCount > 0) {
					append_to_rle(rle, tempCount + byte);
					tempCount = 0;
				}
				else {
					append_to_rle(rle, byte);
				}

				index+= 8;

			} else { // 1 section

				byte &= 0b00000011; //Mask for count
				if (tempCount > 0) {
					append_to_rle(rle, tempCount + byte);
					tempCount = 0;
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

