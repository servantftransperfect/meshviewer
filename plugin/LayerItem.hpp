#pragma once

#include <QObject>
#include <QString>
#include <QVector3D>
#include <qqml.h>
#include <memory>

class IRenderable;

struct LayerPickResult
{
    bool hit = false;
    float distance = 0.0f;
    QVector3D worldPoint;
};

/**
 * @brief Abstract base for all declarative scene layers.
 *
 * Derive from this to create a new layer type. Registered as QML_UNCREATABLE
 * so that only concrete subclasses can be instantiated in QML.
 */
class LayerItem : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("LayerItem is an abstract base class")

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool picking READ picking WRITE setPicking NOTIFY pickingChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:
    explicit LayerItem(QObject *parent = nullptr);
    ~LayerItem() override = default;

    bool visible() const { return _visible; }
    void setVisible(bool v);

    bool picking() const { return _picking; }
    void setPicking(bool picking);

    bool loading() const { return _loading; }
    QString errorString() const { return _errorString; }

    /** @brief Factory method: create the IRenderable that renders this layer. */
    virtual std::unique_ptr<IRenderable> createRenderable() const = 0;

    virtual bool rendersInBackground() const { return false; }
    virtual bool rendersInForeground() const { return false; }

    virtual bool canPick() const { return false; }
    virtual LayerPickResult pick(const struct Ray &ray) const;
    virtual void applyPickResult(const LayerPickResult &result);
    virtual void clearPick();

signals:
    void visibleChanged();
    void pickingChanged();
    void loadingChanged();
    void errorStringChanged();
    /** @brief Emitted when new renderable data is ready to be picked up by the render thread. */
    void dataReady();

protected:
    bool _loading = false;
    QString _errorString;

private:
    bool _visible = true;
    bool _picking = true;
};
