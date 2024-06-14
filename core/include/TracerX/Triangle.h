/**
 * @file Triangle.h
 */
#pragma once

namespace TracerX::core
{

/**
 * @brief Represents a triangle in a mesh.
 */
struct Triangle
{
    /**
     * @brief The index of the first vertex of the triangle.
     */
    int v1;

    /**
     * @brief The index of the second vertex of the triangle.
     */
    int v2;

    /**
     * @brief The index of the third vertex of the triangle.
     */
    int v3;
};

}
