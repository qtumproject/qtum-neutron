//
// Created by CodeFace on 2019/10/29.
//

#include "testvm.h"

VMStatus testvm_execute(const VMContext* context) {
    context->log(context, "testvm executed");
    return VMStatus{VM_SUCCESS};
}
