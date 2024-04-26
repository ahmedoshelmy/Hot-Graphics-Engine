
#include <stdio.h>
#include  "miniaudio/miniaudio.h"

namespace aaa {
     void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) ;

    class MiniAudioWrapper{
        ma_result result;
        ma_decoder decoder;
        ma_device_config deviceConfig;
        ma_device device;
        public:
       

        void playSong(const char* path) ;
    
        void stopSong() ;
    };
}
