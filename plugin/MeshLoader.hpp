#pragma once

#include "MeshData.hpp"

#include <QString>
#include <QVector>
#include <algorithm>
#include <memory>

class MeshLoader
{
public:
    /** @brief Synchronous load — call from a worker thread. */
    static std::unique_ptr<MeshData> load(const QString &path);

private:
    static void computeBounds(MeshData &data);
};