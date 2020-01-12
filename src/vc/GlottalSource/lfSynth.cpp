#include "LF.h"
#include <iostream>

using namespace Eigen;

void lfSynth(const std::vector<LF_Frame> & lff, const ArrayXi & voicing, const ArrayXi & time, double fs, ArrayXd & x)
{
    const int nFrames = lff.size();
    const int nOutput = x.size();

    int count = 1;

    // Find the first frame that is voiced.
    int t, iframe = 0;
    while (iframe < nFrames) {
        if (voicing(iframe)) {
            t = time(iframe);
            break;
        }
        iframe++;
    }

    std::cout << "LF synthesis started." << std::endl;

    while (t < nOutput) {

        int t1 = lfSynthFrame(lff.at(iframe), fs, x, t);

        // Because we're going to be synthesing different pitches, we want to synchronise each cycle with the next. 
        // Find the next frame that is voiced.
        while (iframe < nFrames) {
            if (time(iframe) > t1) {
                if (voicing(iframe)) {
                    break;
                }
                else {
                    //t1 = time(iframe);
                }
            }
            iframe++;
        }

        if (iframe == nFrames) {
            break;
        }

        t = t1;
       
    }
    
    std::cout << "LF synthesis finished." << std::endl;
}
