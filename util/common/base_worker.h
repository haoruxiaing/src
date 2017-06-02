#ifndef  __BASE_WORKER_H_
#define  __BASE_WORKER_H_

/*
 *  all worker base class
 */

class base_worker
{
public:
    base_worker(){};
    virtual ~base_worker(){};
    virtual void setup(){};
    virtual void run_loop(){};
    virtual void clean_up(){};
};

#endif
