// G29-Post-Processor.cpp : main project file.

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#define MINIMUM_GRID_SIZE	40.0			// We always want to probe with this much distance


using namespace System;

void scan_gcode_for_plate_size( char *);
char *strip_comment( char *);
void strip_new_line( char *);
void create_file_with_modified_G29( char *);
void replace_original_file_with_new_G29( char *);
long pause(long);

double x_min=9999.9, x_max=-9999.9, y_min=9999.9, y_max=-9999.9;

int main(int argc, char** argv)
{
	char *file_to_process;


	file_to_process = argv[1];

    	printf("Processing file: %s\n", file_to_process);

	scan_gcode_for_plate_size( file_to_process );

	create_file_with_modified_G29( file_to_process );

	replace_original_file_with_new_G29( file_to_process );

	printf("*Done*\n\n\n");

    return 0;
}

void scan_gcode_for_plate_size( char *filename )
{
int n=0;
errno_t err;
FILE *f=NULL;
float x, y;
char line[1000], original_line[1000], *ptr, *ptr2, *ptr3, *ptr4;

	err = fopen_s( &f, filename, "r");
	if (err != 0) {
		printf("?Unable to open file %s\n", filename);
		exit(0);
	}

	while ( !feof(f) ) {
		n++;
		fgets( line, 999, f);
		strcpy_s(original_line, 999, line);
		strip_comment( line );

		ptr = strstr( line, "G1");
		if ( ptr!=NULL ) {				// Found a G1 command...  Let's check it out!
			ptr2 = strstr(line, "E");
			if ( ptr2>ptr ) {			// There is an Extrustion specified
				ptr3 = strstr(line,"X");
				ptr4 = strstr(line,"Y");
				if (ptr3!=NULL && ptr4!=NULL) {		// we found a set of coordinates!
					sscanf_s( ptr3+1, "%f ", &x);
					if (x>x_max)
						x_max = x;
					if (x<x_min)
						x_min = x;

					sscanf_s( ptr4+1, "%f", &y);
					if (y>y_max)
						y_max = y;
					if (y<y_min)
						y_min = y;
				}
			}
		}
	}
	printf("x_min=%f  x_max=%f y_min=%f  y_max=%f \n", x_min, x_max, y_min, y_max);

	if ( (x_max-x_min)<MINIMUM_GRID_SIZE) {							// widen the grid points if the print is too narrow
		double adjustment=(MINIMUM_GRID_SIZE-(x_max-x_min))/2.0;
		printf("Increasing size of X-Axis grid.\n");
		x_max = x_max + adjustment;
		x_min = x_min - adjustment;
	}

	if ( (y_max-y_min)<MINIMUM_GRID_SIZE) {							// widen the grid points if the print is too narrow
		double adjustment=(MINIMUM_GRID_SIZE-(y_max-y_min))/2.0;
		printf("Increasing size of Y-Axis grid.\n");
		y_max = y_max + adjustment;
		y_min = y_min - adjustment;
	}

	fclose(f);
}


char *strip_comment( char *str )
{
int i, l;

	l = strlen( str );
	for( i=0; i<l; i++) {
		if ( str[i]==';') {
			str[i] = 0;
			return &str[i+1];
			break;
		}
	}
	return NULL;
}

void strip_new_line( char *str )
{
int i, l;

	l = strlen( str );
	for( i=0; i<l; i++) {
		if ( str[i]=='\n') {
			str[i] = 0;
			break;
		}
	}
}


void create_file_with_modified_G29( char *filename )
{
int n=0, G29_already_found_flag=0;
errno_t err;
FILE *f_in=NULL, *f_out=NULL;
char line[1000], temp_file[1000], bed_size[1000], comment[1000], *comment_ptr, *ptr;

	err = fopen_s( &f_in, filename, "r");
	if (err != 0) {
		printf("?Unable to open file %s when trying to create temp file.\n", filename);
		exit(0);
	}

	strcpy_s(temp_file, 999, filename);
	strcat_s(temp_file, 999, "~");
	err = fopen_s( &f_out, temp_file, "w+");
	if (err != 0) {
		printf("?Unable to open temp file %s when trying to create temp file.\n", temp_file);
		exit(0);
	}

	while ( !feof(f_in) ) {
		n++;
		fgets( line, 999, f_in);

		ptr = strstr( line, "G29");
		if ( G29_already_found_flag==0 && ptr!=NULL)	{							// We found the G29 to process!
			G29_already_found_flag++;
			strip_new_line( line );
			comment_ptr = strip_comment( line );
			strcpy_s( comment, 999, comment_ptr);
			strcat_s( line, 999, " ");
			sprintf_s( bed_size, 999, "XMIN %f XMAX %f YMIN %f YMAX %f ", x_min, x_max, y_min, y_max);
			strcat_s( line, 999, bed_size);
			if ( comment != NULL ) {
				strcat_s( line, 999, ";" );	
				strcat_s( line, 999, comment );
			}
			strcat_s( line, 999, "\n");
		}
		fprintf(f_out,"%s", line );
	}
	fclose(f_in);
	fclose(f_out);
}

void replace_original_file_with_new_G29(char *filename )
{
char temp_file[1000];
int status1, status2;

	strcpy_s(temp_file, 999, filename);
	strcat_s(temp_file, 999, "~");

	status1 = remove( filename );
	status2 = rename( temp_file, filename);

	if (status1)
		printf("?Unable to delete file %s.\n", filename);

	if (status2)
		printf("?Unable to rename file %s to %s.\n", temp_file, filename);
}

long pause(long bogus)
{
	return bogus-1;
}
