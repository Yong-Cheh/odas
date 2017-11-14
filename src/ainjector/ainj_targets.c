    
    #include "ainj_targets.h"

    ainj_targets_obj * ainj_targets_construct(const inj_targets_cfg * inj_targets_config, const msg_hops_cfg * msg_hops_config, const msg_targets_cfg * msg_targets_config) {

        ainj_targets_obj * obj;

        obj = (ainj_targets_obj *) malloc(sizeof(ainj_targets_obj));

        obj->inj_targets = inj_targets_construct(inj_targets_config, msg_hops_config, msg_targets_config);

        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_targets_obj *) NULL;

        obj->thread = thread_construct(&ainj_targets_thread, (void *) obj);

        return obj;

    }

    void ainj_targets_destroy(ainj_targets_obj * obj) {

        inj_targets_destroy(obj->inj_targets);
        thread_destroy(obj->thread);

        free((void *) obj);   

    }

    void ainj_targets_connect(ainj_targets_obj * obj, amsg_hops_obj * in, amsg_targets_obj * out) {

        obj->in = in;
        obj->out = out;

    }

    void ainj_targets_disconnect(ainj_targets_obj * obj) {

        obj->in = (amsg_hops_obj *) NULL;
        obj->out = (amsg_targets_obj *) NULL;

    }

    void * ainj_targets_thread(void * ptr) {

        ainj_targets_obj * obj;
        msg_hops_obj * msg_hops_in;
        msg_targets_obj * msg_targets_out;
        int rtnValue;

        obj = (ainj_targets_obj *) ptr;

        while(1) {

            // Pop a message, process, and push back
            msg_hops_in = amsg_hops_filled_pop(obj->in);
            msg_targets_out = amsg_targets_empty_pop(obj->out);
            inj_targets_connect(obj->inj_targets, msg_hops_in, msg_targets_out);
            rtnValue = inj_targets_process(obj->inj_targets);
            inj_targets_disconnect(obj->inj_targets);
            amsg_hops_empty_push(obj->in, msg_hops_in);
            amsg_targets_filled_push(obj->out, msg_targets_out);

            // If this is the last frame, rtnValue = -1
            if (rtnValue == -1) {
                break;
            }

        }


    }