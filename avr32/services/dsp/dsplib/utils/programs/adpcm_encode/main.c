/**
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdio.h>

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long

#define WAVE_FORMAT_DVI_ADPCM  0x0011

#define BLOCK_SIZE   (512/2)
#define SAMPLE_RATE  8000

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  BYTE riff_type[4];
}s_wave_riff;

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  WORD compression_code;
  WORD nb_channels;
  DWORD sample_rate;
  DWORD avg_bytes_per_sec;
  WORD block_align;
  WORD bits_per_sample;
  WORD extra_bytes;
}s_wave_fmt;

typedef struct __attribute__((__packed__))
{
  s_wave_fmt fmt;
  WORD samples_per_block;
}s_wave_fmt_dvi;

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
}s_wave_data;

typedef struct __attribute__((__packed__))
{
  WORD isamp0;
  BYTE step_table_index;
  BYTE reserved;
}s_wave_dvi_block_header;


/*
 * Get a structure from a file starting with the specific string
 */
int fget_struct(FILE *_file, void *_ptr, int size, char *_start_str)
{
  int end;
  char *_str;

  end = 0;
  while(!feof(_file) && !end)
  {
    _str = _start_str;
    while(*_str == fgetc(_file))
    {
      _str++;
      if (!*_str)
      {
        end = 1;
        break;
      }
    }
  }

  if (!end)
    return 0;

  fseek(_file, -strlen(_start_str), SEEK_CUR);
  fread(_ptr, 1, size, _file);

  return 1;
}

int main(int argc, char *_argv[])
{
  FILE *_file, *_file_out;
  s_wave_riff header_riff;
  s_wave_fmt_dvi header_dvi;
  s_wave_data header_data;
  s_wave_dvi_block_header header_block;
  char *_buffer;
  int nb_bytes_per_block;
  int block_size;
  int block_sent = 0;
  int end = 0;
  int file_size;
  int nb_blocks;

  // Check the arguments
  if (argc != 3)
  {
    printf("Usage: ADPCM_IMA_DVI input_file output_file\n");
    return 0;
  }

  printf("Opening input file %s for reading...", _argv[1]);
  fflush(stdout);
  _file = fopen(_argv[1], "rb");
  if (!_file)
  {
    printf("\t[ FAILED ]\n");
    return 0;
  }
  printf("\t[ OK ]\n");

  printf("Opening output file %s for writing...", _argv[2]);
  fflush(stdout);
  _file_out = fopen(_argv[2], "wb");
  if (!_file_out)
  {
    fclose(_file);
    printf("\t[ FAILED ]\n");
    return 0;
  }
  printf("\t[ OK ]\n");

  /* Get input file size */
  fseek(_file, 0, SEEK_END);
  file_size = ftell(_file);
  fseek(_file, 0, SEEK_SET);

  /* Retrieves the number of blocks */
  nb_blocks = file_size/(BLOCK_SIZE + 4);
  /* Set data file size */
  file_size = nb_blocks*256;
  /* Set file size */
  file_size += (sizeof(header_riff) + sizeof(header_dvi) + sizeof(header_data));

  printf("nb_blocks: %i\n", nb_blocks);

  _buffer = malloc((header_dvi.fmt.block_align-sizeof(s_wave_dvi_block_header)));

  /* RIFF Wave header file */
  header_riff.chunk_id[0] = 'R';
  header_riff.chunk_id[1] = 'I';
  header_riff.chunk_id[2] = 'F';
  header_riff.chunk_id[3] = 'F';
  header_riff.chunk_size = file_size - 8;
  header_riff.riff_type[0] = 'W';
  header_riff.riff_type[1] = 'A';
  header_riff.riff_type[2] = 'V';
  header_riff.riff_type[3] = 'E';

  /* fmt DVI header */
  header_dvi.fmt.chunk_id[0] = 'f';
  header_dvi.fmt.chunk_id[1] = 'm';
  header_dvi.fmt.chunk_id[2] = 't';
  header_dvi.fmt.chunk_id[3] = ' ';
  header_dvi.fmt.chunk_size = 0x14;
  header_dvi.fmt.compression_code = WAVE_FORMAT_DVI_ADPCM;
  header_dvi.fmt.nb_channels = 1;
  header_dvi.fmt.sample_rate = SAMPLE_RATE;
  header_dvi.fmt.avg_bytes_per_sec = SAMPLE_RATE/2;
  header_dvi.fmt.block_align = 256;
  header_dvi.fmt.bits_per_sample = 4;
  header_dvi.fmt.extra_bytes = sizeof(header_dvi) - sizeof(header_dvi.fmt);
  header_dvi.samples_per_block = (header_dvi.fmt.block_align - (4*header_dvi.fmt.nb_channels))*8/(header_dvi.fmt.bits_per_sample*header_dvi.fmt.nb_channels)+1;

  /* data DVI header */
  header_data.chunk_id[0] = 'd';
  header_data.chunk_id[1] = 'a';
  header_data.chunk_id[2] = 't';
  header_data.chunk_id[3] = 'a';
  header_data.chunk_size = file_size - sizeof(header_riff) - sizeof(header_dvi) - sizeof(header_data);

  // Support only the 4 bits per sample format.
  nb_bytes_per_block = (header_dvi.fmt.block_align/(4*header_dvi.fmt.nb_channels)-1);
  block_size = nb_bytes_per_block*4;

  // Write the headers
  fwrite(&header_riff, 1, sizeof(s_wave_riff), _file_out);
  fwrite(&header_dvi, 1, sizeof(s_wave_fmt_dvi), _file_out);
  fwrite(&header_data, 1, sizeof(s_wave_data), _file_out);

  printf("File size: %i\n", sizeof(s_wave_riff) + sizeof(s_wave_fmt_dvi) + sizeof(s_wave_data) + (sizeof(s_wave_dvi_block_header) + header_dvi.fmt.block_align-sizeof(s_wave_dvi_block_header))*nb_blocks);

  printf("Creation of the IMA/DVI ADPCM Wave file...");
  fflush(stdout);

  // Main loop
  for(j=0; j<nb_blocks; j++)
  {
    // predicted_value
    fread(&header_block.isamp0, 1, 2, _file);
    // step_index
    fread(&header_block.step_table_index, 1, 2, _file);
    header_block.reserved = 0;

#ifdef __DEBUG
    printf("predicted_value: %i | step_index: %i\n", header_block.isamp0, header_block.step_table_index);
#endif
    // Read the last channel
    fwrite(&header_block, 1, sizeof(s_wave_dvi_block_header), _file_out);

    // Send data
    fread(_buffer, 1, header_dvi.fmt.block_align-sizeof(s_wave_dvi_block_header), _file);
    fwrite(_buffer, 1, header_dvi.fmt.block_align-sizeof(s_wave_dvi_block_header), _file_out);
  }

  printf("\t[ OK ]\n");

  free(_buffer);

  fclose(_file);
  fclose(_file_out);

  return 1;
}
