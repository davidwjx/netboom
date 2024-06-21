#include "inu_app_pipe.h"
#include "inu_utils.h"
#include "ev_init.h"

#define INU_EXP_LUT_SIZE 8193
static float *inu_exp_lut = NULL;
static EvMutexType ocl_mutex[2];


static inu_timing_info timing_info[2][2][MAX_TIMING_ENTRIES]; // 2 sets of 2 cpus * entries
static int timing_info_index[2][2] = { 0, 0 };
static int timing_info_set = 0;

/* exp_lut_init */
void exp_lut_init(void)
{
	float minx, maxx, step_size, val;

	inu_exp_lut = (float *)malloc(sizeof(float)*INU_EXP_LUT_SIZE);
	if (inu_exp_lut) {
		minx = -6;
		maxx = 2;
		step_size = abs(minx - maxx) / INU_EXP_LUT_SIZE;
		for (int i = 0; i < INU_EXP_LUT_SIZE; i++) {
			val = minx + step_size*i;
			inu_exp_lut[i] = exp(val);
		}
	}
	else {
		LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to allocate memory for inu_exp_lut\n");
	}
}

/* inu_exp */
float inu_exp(float val)
{
	float result;
	int index;
	static float  step_size = 9.765625e-04F; //abs(Max_x - Min_x) / Lut_size = 9.765625e-04

	if (val >= 2) {
		result = powf(E_CONST, val);
	}
	else {
		if (val < -6) val = -6;
		index = (int)(val / step_size + 0.5) + 6145;
		result = inu_exp_lut[index];
	}
	return result;
}

/* overlap */
float overlap(float x1, float w1, float x2, float w2)
{
	float l1 = x1 - w1 / 2;
	float l2 = x2 - w2 / 2;
	float left = l1 > l2 ? l1 : l2;
	float r1 = x1 + w1 / 2;
	float r2 = x2 + w2 / 2;
	float right = r1 < r2 ? r1 : r2;
	return right - left;
}

/* box_intersection */
float box_intersection(Box a, Box b)
{
	float w = overlap(a.x, a.w, b.x, b.w);
	float h = overlap(a.y, a.h, b.y, b.h);
	if (w < 0 || h < 0) return 0;
	float area = w*h;
	return area;
}

/* box_union */
float box_union(Box a, Box b)
{
	float i = box_intersection(a, b);
	float u = a.w*a.h + b.w*b.h - i;
	return u;
}

/* box_iou */
float box_iou(Box a, Box b)
{
	return box_intersection(a, b) / box_union(a, b);
}

/* inuCallOcl */
void inuCallOcl(ev_ocl_kernel_data d, void **args)
{
	evMutexLock(&ocl_mutex[evGetCpu()]);
	callOclManualKernel(d, args);
	evMutexUnLock(&ocl_mutex[evGetCpu()]);
}

/* Init ocl mutexes */
int init_ocl_mutexes(void)
{
	int rc, i;

	for (i = 0; i < 2; i++) {
		rc = evMutexInit(&ocl_mutex[i]);
		if (rc != EVTH_ERROR_OK)
		{
			LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "ev_inu_api_init: Failed to create ocl_mutex %d\n", i);
			return(EV_CNN_STATUS_FAIL);
		}
	}

	return(EV_CNN_STATUS_OK);
}

/* inu_init_timing_info */
void inu_init_timing_info(void)
{
#ifdef INU_DEBUG_MEAS
	timing_info_set = 1 - timing_info_set;
	timing_info_index[timing_info_set][0] = 0;
	timing_info_index[timing_info_set][1] = 0;
	for (int cpu_ind = 0; cpu_ind < 2; cpu_ind++) {
		for (int i = 0; i < MAX_TIMING_ENTRIES; i++) {
			timing_info[timing_info_set][cpu_ind][i].start_time = 0;
			timing_info[timing_info_set][cpu_ind][i].end_time = 0;
			timing_info[timing_info_set][cpu_ind][i].text[0] = '\0';
		}
	}
#endif
}

/* inu_register_timing_info */
void inu_register_timing_info(uint64_t start_time, uint64_t end_time, char* text)
{
#ifdef INU_DEBUG_MEAS
	int cpu_id = evGetCpu();
	int ind;

	if (timing_info_index[timing_info_set][cpu_id] >= MAX_TIMING_ENTRIES) {
		return;
	}

	ind = timing_info_index[timing_info_set][cpu_id];

	timing_info[timing_info_set][cpu_id][ind].start_time = start_time;
	timing_info[timing_info_set][cpu_id][ind].end_time = end_time;
	timing_info[timing_info_set][cpu_id][ind].thread = evGetRunningThread();
	strncpy(timing_info[timing_info_set][cpu_id][ind].text, text, MAX_TIMING_TEXT_LEN-1);
	timing_info_index[timing_info_set][cpu_id]++;
#endif
}

/* inu_print_timing_info */
void inu_print_timing_info(bool flush_flag)
{
#ifdef INU_DEBUG_MEAS
	int index[2] = { 0, 0 };
	int print_cpu = 0;
	float ms;
	float ev_clock_rate = EV_INITG_getEvFreqMhz();
	uint64_t start_time, end_time;
	int num_lines = 0;
	int set;

	set = timing_info_set;

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "Timing entries set %d: %d+%d at %dMhz\n", set, timing_info_index[set][0], timing_info_index[set][1], (int)ev_clock_rate);

	while ((index[0] < timing_info_index[set][0] && timing_info_index[set][0] > 0) || (index[1] < timing_info_index[set][1] && timing_info_index[set][1] > 0)) {

		if (timing_info[set][0][index[0]].end_time < timing_info[set][1][index[1]].end_time && timing_info[set][0][index[0]].end_time > 0 && index[0] < timing_info_index[set][0] || index[1] >= timing_info_index[set][1]) {
			print_cpu = 0;
		}
		else if (timing_info[set][1][index[1]].end_time < timing_info[set][0][index[0]].end_time && timing_info[set][1][index[1]].end_time > 0 && index[1] < timing_info_index[set][1] || index[0] >= timing_info_index[set][0]) {
			print_cpu = 1;
		}
		else {
			LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(%d) %lld %lld ::(%d) %lld %lld\n", index[0], timing_info[set][0][index[0]].thread, timing_info[set][0][index[0]].start_time, timing_info[set][0][index[0]].end_time, index[1], timing_info[set][1][index[1]].start_time, timing_info[set][1][index[1]].end_time);
		}

		start_time = timing_info[set][print_cpu][index[print_cpu]].start_time;
		end_time = timing_info[set][print_cpu][index[print_cpu]].end_time;


		ms = (float)(end_time - start_time) / (ev_clock_rate * 1000);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "[cpu %d - %p] - %s: from:%lld to:%lld ms:%f\n", print_cpu, timing_info[set][print_cpu][index[print_cpu]].thread, timing_info[set][print_cpu][index[print_cpu]].text, start_time, end_time, ms);
		//LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "[cpu %d - %p] - %s\n", print_cpu, timing_info[set][print_cpu][index[print_cpu]].thread, timing_info[set][print_cpu][index[print_cpu]].text);
		index[print_cpu]++;
		num_lines++;
		if (num_lines > 10) {
			if(!flush_flag) evSleepThread(864000 * 50);
			num_lines = 0;
		}
	}

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "-----\n");

	inu_init_timing_info();
#endif

}