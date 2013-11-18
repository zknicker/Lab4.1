//
//  normal_utils.h
//  Shadows
//
//  Created by Zach Knickerbocker on 11/18/13.
//  Copyright (c) 2013 Zach Knickerbocker. All rights reserved.
//

#ifndef NORMAL_UTILS_H
#define NORMAL_UTILS_H

void computeTangentBasis(
    std::vector<glm::vec3> & vertices,
    std::vector<glm::vec2> & uvs,
    std::vector<glm::vec3> & normals,
    std::vector<glm::vec3> & tangents,
    std::vector<glm::vec3> & bitangents
);


#endif
