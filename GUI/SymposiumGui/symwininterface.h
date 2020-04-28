#ifndef SCREENINTERFACE_H
#define SCREENINTERFACE_H
#include<QString>
#include<QWidget>
class clientDispatcher;

class SymWinManager;

struct SymNotepadWinInterface;

namespace Symposium {
    struct symbol{
        void dummyMethod() const{}
    };
}


struct isQWidget{
    class QWidgetType{
        QWidgetType(){}
        friend struct isQWidget;
    };
    template<class T>
    static QWidgetType isQwidgetType(const T&){
        static_assert (std::is_base_of<QWidget, T>::value, "Error: not a QWidget");
        return QWidgetType();
    }
};

/**
 * @brief Base class used for a window in SymposiumGUI
 *
 * Every window in the system must provide the actions @e success and @e failure,
 * to receive communication from the @ref clientDispatcher on the outcome of
 * the last action required by the user. Every window deriving from SymWinInterface
 * must derive from QWidget (or subclasses)
 */
class SymWinInterface{
    friend struct SymChildWinInterface;
    friend struct SymMainWinInterface;
    friend struct SymNotepadWinInterface;
protected:
    SymWinManager& handler;                 /**< a reference to the windows handler, used to inform when this window is opening another one */
    clientDispatcher& cl;                /**< a reference to the @ref clientDispatcher to whom actions are to be requested */
private:
    bool forceQuit;                      /**< indicates whether the window is closed by the code (true) or explicitly by the user. False by default */
    SymWinInterface* s_parent;           /**< the window that opened this window*/

    /**
     * @brief SymWinInterface private constructor used by @ref SymChildWinInterface and @ref SymMainWinInterface (friends) subclasses to create a window
     * @param m reference to the @ref winManager the window will use
     * @param p reference to the @ref clientDispatcher the window will use
     */
    SymWinInterface(SymWinManager& m, clientDispatcher& p);

    /**
     * @brief operator QWidget * used to show, close or hide a Qt window.
     * Every window deriving from SymWinInterface must derive from QWidget (or subclasses)
     */
    explicit operator QWidget*();

public:
    /**
     * @brief success method to be called when the last action required by the user on this window
     * has been completed successfully.
     */
    virtual void success()=0;

    /**
     * @brief failure method to be called when the last action required by the user on this window
     * had errors
     * @param toPrint an error message to shw onto the screen
     */
    virtual void failure(const QString& toPrint)=0;

protected:
    /**
     * @brief closedByUser indicates whether the window is closed by the user
     * @return true if the window has been closed because of an explicit close by the user
     */
    bool closedByUser() const;

    /**
     * @brief goToWindow hides the current, set @ref nextWin as active in the @ref handler and shows it
     * @param nextWin the window to be shown
     */
    void goToWindow(SymWinInterface& nextWin);

    /**
     * @brief goToWindow this is an overloaded version for @ref SymNotepadInterface windows.
     * As @ref goToWindow(SymWinInterface*), but also registers the notepad to the handler
     * @param notepad the SymNotepadInterface window to be shown
     */
    void goToWindow(SymNotepadWinInterface& notepad);


    virtual ~SymWinInterface()= default;
};


/**
 * @brief base class used forSymposiumGUI windows that must have a parent screen
 */
struct SymChildWinInterface : public SymWinInterface{
protected:
    SymChildWinInterface(SymWinInterface& parentScreen, isQWidget::QWidgetType);
    /**
     * @brief backToParent sets the parent window as active, shows it and closes the current window
     */
    void backToParent();
    /**
     * @brief backToMainWin closes every window opened after the main window, sets it as active and shows it
     */
    void backToMainWin();
};


/**
 * @brief base class used forSymposiumGUI windows that haven't to have a parent screen
 */
struct SymMainWinInterface: public SymWinInterface{
    SymMainWinInterface(SymWinManager& m, clientDispatcher& p, isQWidget::QWidgetType);
};

struct SymNotepadWinInterface: public SymChildWinInterface{
    SymNotepadWinInterface(SymWinInterface& parentScreen, isQWidget::QWidgetType arg);
    virtual int getId()=0;
    virtual void successfullInsert(const Symposium::symbol& sym)=0;
    virtual void failedInsert(const Symposium::symbol& sym)=0;
    virtual void failedRemove(const Symposium::symbol& sym)=0;
    /**
     * @brief forceClose used by the SymWinManager to close all this SymNotepadWinInterface window forcefully
     */
    void forceClose();
protected:
    /**
     * @brief showParent sets the parent window, that should be a directory window,
     * as active, and shows it without closing the current SymNotepadInterface window
     */
    void showParent();
    /**
     * @brief focusToDirectory sets the parent window, that should be a directory window,
     * as active. It is to be called when the focus goes to the directory.
     */
    void focusToDirectory();
    /**
     * @brief obtainFocus sets the current window as active and minimizes the parent, the directory
     * window, if currently shown. It is to be called when the focus goes the current window.
     */
    void obtainFocus();
    /**
     * @brief closeNotepad notifies the SymWinManager that the current notepad has been closed.
     * It is to be called when user closes the current window
     */
    void closeNotepad();
};

#endif // SCREENINTERFACE_H
