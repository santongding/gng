#include "data-ops.h"
file_single * get_file_by_loc(file_location loc){
    if(loc.commit == 0){
        return NULL;
    }else{
        exit(1);
    }
}