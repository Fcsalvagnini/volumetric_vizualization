#include "ift.h"
#include "get_kernels.h"
#include "convolution.h"
#include "segmentation.h"
#include "assess.h"

int main(int argc, char *argv[]) {
    timer *t_start = NULL;
    int mem_din_start, mem_din_end;

    mem_din_start = iftMemoryUsed();

    if (argc < 2) {
        iftError(
            "Usage: main <...>\n"
            "[1] mode: generate kernels, convolve, segment, assess [0|1|2|3]. \n",
            "main"
        );
    }

    t_start = iftTic();

    if (atoi(argv[1]) == 0) {
        GetKernels(argc, argv);
    }
    else if (atoi(argv[1]) == 1) {
        GetFeatureMaps(argc, argv);
    }
    else if (atoi(argv[1]) == 2) {
        RunSegmentation(argc, argv);
    }
    else if (atoi(argv[1]) == 3) {
        AssessSegmentations(argc, argv);
    }

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(t_start, iftToc())));
    mem_din_end = iftMemoryUsed();
    iftVerifyMemory(mem_din_start, mem_din_end);

    return 0;
}