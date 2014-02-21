#include <CL/cl.h>
#include <string>

namespace dfgenerator {

template<class T, const size_t N>
class OpenCL
{
public:

    OpenCL()
    {
        cl_device_id device_id = NULL;
        cl_context context = NULL;
        cl_command_queue command_queue = NULL;
        cl_mem memobj = NULL;
        cl_program program = NULL;
        cl_kernel kernel = NULL;
        cl_platform_id platform_id = NULL;
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;
        cl_int ret;

        /* Get Platform and Device Info */
        ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
        ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

        /* Create OpenCL context */
        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

        /* Create Command Queue */
        command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

        /* Create Memory Buffer */
        memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, N * sizeof(T), NULL, &ret);

        /* Create Kernel Program from the source */

            const char *p =
                    "__kernel void hello(__global char* string)"
                    "{"
                    "string[0] = 'H';"
                    "string[1] = 'e';"
                    "string[2] = 'l';"
                    "string[3] = 'l';"
                    "string[4] = 'o';"
                    "string[5] = ',';"
                    "string[6] = ' ';"
                    "string[7] = 'W';"
                    "string[8] = 'o';"
                    "string[9] = 'r';"
                    "string[10] = 'l';"
                    "string[11] = 'd';"
                    "string[12] = '!';"
                    "string[13] = '\0';"
                    "}"
                    ;
            const size_t s = strlen(p);
            program = clCreateProgramWithSource(context, 1, &p, &s, &ret);

        /* Build Kernel Program */
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

        /* Create OpenCL Kernel */
        kernel = clCreateKernel(program, "hello", &ret);

        /* Set OpenCL Kernel Parameters */
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);

        /* Execute OpenCL Kernel */
        ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);

        char *string = (char*) malloc(N * sizeof(T));

        /* Copy results from the memory buffer */
        ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
                                  N * sizeof(T),string, 0, NULL, NULL);

        /* Display Result */
        std::cout << string << std::endl;

        /* Finalization */
        ret = clFlush(command_queue);
        ret = clFinish(command_queue);
        ret = clReleaseKernel(kernel);
        ret = clReleaseProgram(program);
        ret = clReleaseMemObject(memobj);
        ret = clReleaseCommandQueue(command_queue);
        ret = clReleaseContext(context);

    }

};

}
