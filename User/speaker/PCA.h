#ifndef __PCA_H
#define	__PCA_H


#define PWM_8KHZ			(8999)
#define PWM_80KHZ		(899)

void PCA_init(void);
unsigned char PCA_buf_fill_validate(void);
void PCA_buf_fill_code(uint8_t pcm); 

void PCA_Test_SampleVox(void);

#endif

