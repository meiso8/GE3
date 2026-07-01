#include "Crowbar.h"
#include "../SunRod/SunRod.h"
#include"CoordinateTransform.h"
#include"Easing.h"

std::shared_ptr<Item>  Crowbar::CombineWith(const std::shared_ptr<Item>& other)  {
    if (other->GetName() == "SunMedal") {
        //メダルと合体するとSunRod
        return std::make_shared<SunRod>();
    }
    return nullptr;
}

