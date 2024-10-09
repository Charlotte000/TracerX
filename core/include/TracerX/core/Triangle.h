/**
 * @file Triangle.h
 */
#pragma once

namespace TracerX::core
{

/**
 * @brief A collection of three indices of Scene::vertices that form a triangle.
 */
struct Triangle
{
    /**
     * @brief The index of the first Vertex of the triangle.
     */
    int v1;

    /**
     * @brief The index of the second Vertex of the triangle.
     */
    int v2;

    /**
     * @brief The index of the third Vertex of the triangle.
     */
    int v3;
};

}
