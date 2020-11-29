#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#if WINDOWS
#include <windows.h>
#pragma comment(lib, "winmm.lib")
typedef unsigned long my_time;
#endif

/* ���̃R�[�h�̖ړI:
 * ����������CPU�ɓ]������f�[�^���Ȃ�ׂ��A���̈�ɂ�������
 * CPU�������������ł��邱�Ƃ����؂���
 */

typedef struct _t_field {
	double val;
} t_field;

#define SEED 10
#define COLUMN_NUM 100
#define HEADERSZ (COLUMN_NUM * sizeof(t_field))

typedef struct _t_record {
	int field_offset[COLUMN_NUM];
	char vals[COLUMN_NUM * sizeof(t_field)];
} t_record;

typedef struct _t_column {
	t_field* field;
} t_column;

//���R�[�h
/*
10,000,000���R�[�h�~100�J�����~8B���������_=8GB
*/

my_time get_time(void) {
#if WINDOWS
	return timeGetTime();
#endif
}

void tuple_oriented(int record_num, int column_period) {
	t_record *array_record;
	int seed = SEED;
	int record_no, column_no;
	t_field *array_result = NULL;
	DWORD start, end;

	//���R�[�h�̔z����m��
	array_record = (t_record*)calloc(1, sizeof(t_record) * record_num);

	//���ʏo�͗p�̔z����m��
	array_result = (t_field*)calloc(1, sizeof(t_field) * COLUMN_NUM);

	//�[���������g���ăJ�����̒l���쐬
	srand(SEED);
	for (record_no = 0; record_no < record_num; record_no++) {
		for (column_no = 0; column_no < COLUMN_NUM; column_no++) {
			int offset = 0;
			t_field field = { 0 };
			offset = (column_no * sizeof(t_field));
			array_record[record_no].field_offset[column_no] = offset;
			field.val = 0.0001 * ((double)rand() / RAND_MAX);
			memcpy(&(array_record[record_no].vals[offset]), &field, sizeof(field));
		}
	}

	start = get_time();
	fflush(stdout);
	printf("start:%lu\n", start);
	for (record_no = 0; record_no < record_num; record_no++) {
		for (column_no = 0; column_no < COLUMN_NUM; column_no += column_period) {
			int offset = 0;
			t_field field = { 0 };
			
			offset = array_record[record_no].field_offset[column_no];
			memcpy(&field, (t_field*)&(array_record[record_no].vals[offset]), sizeof(field));
			array_result[column_no].val += field.val;
		}
	}
	end = get_time();

	fflush(stdout);
	printf("end:%lu\n", end);
	printf("elapsed:%lu\n",end - start);

	//���ʂ��o��
	for (column_no = 0; column_no < COLUMN_NUM; column_no += column_period) {
		printf("column_no %d:%lf\n", column_no, array_result[column_no].val);
	}

	free(array_record);

}

void column_oriented(int record_num, int column_period) {
	t_column array_column[COLUMN_NUM];
	int seed = SEED;
	int record_no, column_no;
	t_field* array_result = NULL;
	DWORD start, end;

	//�J�����̔z����m��
	for (column_no = 0; column_no < COLUMN_NUM; column_no++) {
		array_column[column_no].field = (t_field*)calloc(1, sizeof(t_field) * record_num);
	}

	//���ʏo�͗p�̔z����m��
	array_result = (t_field*)calloc(1, sizeof(t_field) * COLUMN_NUM);

	//�[���������g���ăJ�����̒l���쐬
	srand(SEED);
	for (column_no = 0; column_no < COLUMN_NUM; column_no++) {
		for (record_no = 0; record_no < record_num; record_no++) {
			array_column[column_no].field[record_no].val
				= 0.0001 * ((double)rand() / RAND_MAX);
		}
	}

	start = get_time();
	fflush(stdout);
	printf("start:%lu\n", start);
	for (column_no = 0; column_no < COLUMN_NUM; column_no += column_period) {
		for (record_no = 0; record_no < record_num; record_no++) {
			array_result[column_no].val
				+= array_column[column_no].field[record_no].val;
		}
	}
	end = get_time();

	fflush(stdout);
	printf("end:%lu\n", end);
	printf("elapsed:%lu\n", end - start);

	//���ʂ��o��
	for (column_no = 0; column_no < COLUMN_NUM; column_no += column_period) {
		printf("column_no %d:%lf\n", column_no, array_result[column_no].val);
	}

}

void main(int argc, char* argv[]) {
	int record_num, column_period;

	if (argc < 3) {
		fprintf(stderr, "cpu_test <record_num> <column_period>\n");
	}

	record_num = atoi(argv[1]);
	column_period = atoi(argv[2]);
	tuple_oriented(record_num, column_period);
	column_oriented(record_num, column_period);
}

