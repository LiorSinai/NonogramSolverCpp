/*
 *  Created on: 7 November 2020
 *      Author: Lior Sinai
 */

#ifndef COMMON_CLASSES_H
#define COMMON_CLASSES_H

#include <vector>

enum cell_types {BOX=1, BLANK=2, EITHER=3, DUMMY=4};
enum cell_types2 {BLACK=1, WHITE=2, EITHER_=3};

struct Match
{
    std::vector<int> match{};
    std::vector<int> pattern{};
    bool is_match {false};
};

#endif 