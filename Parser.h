/*Specific parser code for communication with Python app/code for
 * Lase engraving exhibit , 3D Exhibition, Bloomfield Science Museum 
 * Develop June 2023, Amitai and Amir Ben-Shalom 
 * Led on/off, Move to XY (mm, float 2digit accuracy..), 4points cubic Bezier Curve...
 * 
 */

#ifndef PARSER_H
#define PARSER_H

#include "Basic_routines.h"

void process_in_command();
void print_help_menu();
void read_xyz();
void read_rate();
void read_power();
byte get_in_command();

#endif /* PARSER_H */
