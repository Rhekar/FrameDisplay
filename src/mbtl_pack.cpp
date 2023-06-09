// mbtl_pack:
//
// File loader for MBTL PC edition.

#include "mbtl_pack.h"

#include <cstring>
#include <cstdlib>

// decryption function. Only applies to first 4096 bytes of data.
void decrapt(unsigned char *data, unsigned int size,
		unsigned int xorkey, unsigned int xormod) {
    data[0] ^= 0xA5;
    data[1] ^= 0x18;

    uint32_t A = data[0] ^ 0xAC;
    uint32_t B = data[0] ^ 0xAC ^ data[1] ^ 0x76381;

    if (size > 2 )
    {
        for (uint32_t i = size - 1; i > 1; i--) {
            data[i] ^= ENTRY_KEY[A ^ B & 0x3FF];
            B++;
        }
    }
}

bool MBTL_Pack::open_pack(const char *filename) {
	if (m_file) {
		return 0;
	}

	FILE *f;

	f = fopen(filename, "rb");
	if (!f) {
		return 0;
	}

  /*
	struct header_t {
		unsigned char string[16];
		unsigned int flag;
		unsigned int xor_key;
		unsigned int table_size;
		unsigned int data_size;
		unsigned int folder_count;
		unsigned int file_count;
		unsigned int unknown[3];
	} header;

	if (fread(&header, sizeof(header), 1, f) <= 0) {
		fclose(f);
		return 0;
	}

	if (memcmp(header.string, "FilePacHeaderA", 14)) {
		fclose(f);
		return 0;
	}

	if (header.folder_count > 10000 || header.file_count > 10000) {
		fclose(f);
		return 0;
	}

	// read the folder/file index in
	FolderIndex *folder_index = new FolderIndex[header.folder_count];

	fread(folder_index, header.folder_count, sizeof(FolderIndex), f);
	m_data_folder_id = 32768;

	for (unsigned int i = 0; i < header.folder_count; ++i) {
		decrapt(folder_index[i].filename, 256, header.xor_key, folder_index[i].size);

		if (!strcmp((char *)folder_index[i].filename, ".\\data")) {
			m_data_folder_id = i;
		}
	}

	if (m_data_folder_id == 32768) {
		delete[] folder_index;

		fclose(f);

		return 0;
	}

	FileIndex *file_index = new FileIndex[header.file_count];
	fread(file_index, header.file_count, sizeof(FileIndex), f);

	for (unsigned int i = 0; i < header.file_count; ++i) {
		decrapt(file_index[i].filename, 32, header.xor_key, file_index[i].size);
	}

	// finish up!
	m_data_start = header.table_size;
	m_xor_key = header.xor_key;

	m_folder_index = folder_index;
	m_file_index = file_index;

  */
	m_file = f;

	return 1;
}

void MBTL_Pack::close_pack() {
	if (!m_file) {
		return;
	}

	fclose(m_file);
	m_file = 0;

	if (m_folder_index) {
		delete[] m_folder_index;
		m_folder_index = 0;
	}
	m_folder_count = 0;

	if (m_file_index) {
		delete[] m_file_index;
		m_file_index = 0;
	}
	m_file_count = 0;
}

bool MBTL_Pack::read_file(const char *filename, char **dest, unsigned int *dsize, int bsize, int offset) {
  // TODO: actually read from pack

    /*
  FILE *f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  if ( fsize < 0 ) {
      return 0;
  }
  *dsize = abs( fsize );
  fseek(f, 0, SEEK_SET);
  unsigned char *string = new unsigned char[fsize + 3];
  fread(string, fsize, 1, f);
  fclose(f);
  string[fsize] = 0;
  *dest = (char *)string;
  //return 1;

  //FILE *fp2 = fopen ("pack.txt", "wb");
  //fprintf( fp2, "%s: %d %d\n", filename, bsize, offset );
  //fwrite ( data , sizeof(char), 10, fp2);
  */

  unsigned char *data = new unsigned char[bsize + 3];
	fseek(m_file, offset, SEEK_SET);

  int count = fread(data, bsize, 1, m_file);

  //fprintf( fp2, "%s: %d\n", filename, count );
  //fclose( fp2 );
  if (count >= 1) {
      decrapt(data, bsize, m_xor_key, 0x03);

      data[bsize] = '\0';

      *dest = (char *)data;
      *dsize = bsize;

      return 1;
  }

  /*
	// hacky, but it's in the middle so it doesn't matter.
	unsigned int n = m_folder_index[m_data_folder_id].file_start_id;

	unsigned int folder = m_data_folder_id + 1;
	unsigned int n_end;

	do {
		if (folder == m_folder_count) {
			n_end = m_file_count;
		} else {
			n_end = m_folder_index[folder++].file_start_id;
		}
	} while (n_end == 0);

	while (n < n_end) {
		if (!strcasecmp((char *)m_file_index[n].filename, filename)) {
			// Found it. Read it in.
			unsigned char *data = new unsigned char[m_file_index[n].size + 3];

			fseek(m_file, m_data_start + m_file_index[n].pos, SEEK_SET);

			int count = fread(data, m_file_index[n].size, 1, m_file);

			if (count >= 1) {
				decrapt(data, m_file_index[n].size, m_xor_key, 0x03);

				data[m_file_index[n].size] = '\0';

				*dest = (char *)data;
				*dsize = m_file_index[n].size;

				return 1;
			}

			return 0;
		}

		n += 1;
	}
  */
	return 0;
}

MBTL_Pack::MBTL_Pack() {
	m_file = 0;

	m_folder_index = 0;
	m_folder_count = 0;

	m_file_index = 0;
	m_file_count = 0;
}

MBTL_Pack::~MBTL_Pack() {
	close_pack();
}
