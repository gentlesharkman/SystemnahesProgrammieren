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
     */
    bool type = 0;                  //Which type is encoded by the section, 0 or 1
    uint8_t serialByte;             //
    char *serialResult;             //Result data of serialization
    uint64_t serialResultIndex = 0; //Current index in the serialized string
    uint8_t halfbyte;               //Temporarily saves half a byte of serialization
    int resultSize;

    for(RLENode* currentNode = rle->head; currentNode != NULL ; currentNode = currentNode->next) { //Loops through all the nodes
        uint64_t secCount = currentNode->count; //

        if(secCount == 0) { //Current node is done -> the type that's encoded switches
            type += type % 2;
        }
        while(secCount > 0) {

            if(secCount >= 64) {
                resultSize += 2;
                if(type == 0) { serialByte | 1; } //Set type to first bit
                else { serialByte & 0; }
                serialByte << 6;
                serialByte | 63;
                secCount -= 63;

                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    halfbyte << 4;
                    halfbyte | temp;
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    serialByte << 4;
                    halfbyte = serialByte;
                }
                
                else { serialResult[serialResultIndex++] = (char) serialByte; }
            }

            else if(secCount > 3) {
                resultSize += 2;
                if(type == 0) { serialByte | 1; }
                else { serialByte & 0; }
                serialByte << 6;
                serialByte | secCount;
                secCount = 0;

                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    halfbyte << 4;
                    halfbyte | temp;
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    serialByte << 4;
                    halfbyte = serialByte;
                }

                else { serialResult[serialResultIndex++] = (char) serialByte; }
            }

            else {
                resultSize += 1;
                if(type == 0) { serialByte | 1; }
                else { serialByte & 0; }
                serialByte << 2;
                serialByte | secCount;
                secCount = 0;
                
                if(halfbyte != 0) {
                    uint8_t temp = serialByte;
                    temp >> 4;
                    halfbyte << 4;
                    halfbyte | temp;
                    serialResult[serialResultIndex++] = (char) halfbyte;
                    serialByte << 4;
                    if(serialByte == 0) {
                        serialResult[serialResultIndex++] = (char) halfbyte;
                    }
                    halfbyte = serialByte;
                }

            }
            
        }
    }
    serialResult[serialResultIndex++] = '\0';
    char* result = malloc(resultSize/2 + 1);
    *result = serialResult;
    return result;
}

void deserialize_rle(RLE *rle, const char *data, size_t size) {
   
}
