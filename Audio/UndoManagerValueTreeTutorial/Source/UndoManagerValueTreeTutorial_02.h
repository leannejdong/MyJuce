/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             UndoManagerValueTreeTutorial
 version:          2.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Enables undo and redo features on a value tree.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2019, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class ValueTreeItem  : public juce::TreeViewItem,
                       private juce::ValueTree::Listener
{
public:
    ValueTreeItem (const juce::ValueTree& v, juce::UndoManager& um)
        : tree (v), undoManager (um)    // [3]
    {
        tree.addListener (this);
    }

    juce::String getUniqueName() const override
    {
        return tree["name"].toString();
    }

    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }

    void paintItem (juce::Graphics& g, int width, int height) override
    {
        g.setColour (juce::Colours::white);
        g.setFont (15.0f);

        g.drawText (tree["name"].toString(),
                    4, 0, width - 4, height,
                    juce::Justification::centredLeft, true);
    }

    void itemOpennessChanged (bool isNowOpen) override
    {
        if (isNowOpen && getNumSubItems() == 0)
            refreshSubItems();
        else
            clearSubItems();
    }

    juce::var getDragSourceDescription() override
    {
        return "Drag Source";
    }

    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description == "Drag Source";
    }

    void itemDropped (const juce::DragAndDropTarget::SourceDetails&, int insertIndex) override
    {
        juce::OwnedArray<juce::ValueTree> selectedTrees;
        getSelectedTreeViewItems (*getOwnerView(), selectedTrees);

        moveItems (*getOwnerView(), selectedTrees, tree, insertIndex, undoManager);     // [1]
    }

    static void moveItems (juce::TreeView& treeView, const juce::OwnedArray<juce::ValueTree>& items,
                           juce::ValueTree newParent, int insertIndex, juce::UndoManager& undoManager)
    {
        if (items.size() > 0)
        {
            std::unique_ptr<juce::XmlElement> oldOpenness (treeView.getOpennessState (false));

            for (auto i = items.size(); --i >= 0;)
            {
                auto& v = *items.getUnchecked(i);

                if (v.getParent().isValid() && newParent != v && ! newParent.isAChildOf (v))
                {
                    if (v.getParent() == newParent && newParent.indexOf(v) < insertIndex)
                        --insertIndex;

                    v.getParent().removeChild (v, &undoManager);                        // [2]
                    newParent.addChild (v, insertIndex, &undoManager);                  // [3]
                }
            }

            if (oldOpenness != nullptr)
                treeView.restoreOpennessState (*oldOpenness, false);
        }
    }

    static void getSelectedTreeViewItems (juce::TreeView& treeView, juce::OwnedArray<juce::ValueTree>& items)
    {
        auto numSelected = treeView.getNumSelectedItems();

        for (auto i = 0; i < numSelected; ++i)
            if (auto* vti = dynamic_cast<ValueTreeItem*> (treeView.getSelectedItem (i)))
                items.add (new juce::ValueTree (vti->tree));
    }

private:
    juce::ValueTree tree;
    juce::UndoManager& undoManager; // [2]

    void refreshSubItems()
    {
        clearSubItems();

        for (auto i = 0; i < tree.getNumChildren(); ++i)
            addSubItem (new ValueTreeItem (tree.getChild (i), undoManager));    // [4]
    }

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override
    {
        repaintItem();
    }

    void valueTreeChildAdded        (juce::ValueTree& parentTree, juce::ValueTree&)      override { treeChildrenChanged (parentTree); }
    void valueTreeChildRemoved      (juce::ValueTree& parentTree, juce::ValueTree&, int) override { treeChildrenChanged (parentTree); }
    void valueTreeChildOrderChanged (juce::ValueTree& parentTree, int, int)              override { treeChildrenChanged (parentTree); }
    void valueTreeParentChanged     (juce::ValueTree&)                                   override {}

    void treeChildrenChanged (const juce::ValueTree& parentTree)
    {
        if (parentTree == tree)
        {
            refreshSubItems();
            treeHasChanged();
            setOpen (true);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeItem)
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public juce::Component,
                               public juce::DragAndDropContainer,
                               private juce::Timer      // [1]
{
public:
    //==============================================================================
    MainContentComponent()
        : undoButton ("Undo"),
          redoButton ("Redo")   // [2]
    {
        addAndMakeVisible (tree);

        tree.setDefaultOpenness (true);
        tree.setMultiSelectEnabled (true);
        rootItem.reset (new ValueTreeItem (createRootValueTree(), undoManager));    // [5]
        tree.setRootItem (rootItem.get());

        addAndMakeVisible (undoButton);
        addAndMakeVisible (redoButton);                         // [3]
        undoButton.onClick = [this] { undoManager.undo(); };
        redoButton.onClick = [this] { undoManager.redo(); };    // [4]

        setSize (600, 400);

        startTimer (500);       // [3]
    }

    ~MainContentComponent() override
    {
        tree.setRootItem (nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.

        auto r = getLocalBounds();

        auto buttons = r.removeFromBottom (20);
        undoButton.setBounds (buttons.removeFromLeft (100));
        redoButton.setBounds (buttons.removeFromLeft (100));

        tree.setBounds (r);
    }

    //==============================================================================
    static juce::ValueTree createTree (const juce::String& desc)
    {
        juce::ValueTree t ("Item");
        t.setProperty ("name", desc, nullptr);
        return t;
    }

    static juce::ValueTree createRootValueTree()
    {
        auto vt = createTree ("ValueTree");

        auto n = 1;
        vt.addChild (createRandomTree (n, 0), -1, nullptr);

        return vt;
    }

    static juce::ValueTree createRandomTree (int& counter, int depth)
    {
        auto t = createTree ("Item " + juce::String (counter++));

        if (depth < 3)
            for (auto i = 1 + juce::Random::getSystemRandom().nextInt (7); --i >= 0;)
                t.addChild (createRandomTree (counter, depth + 1), -1, nullptr);

        return t;
    }

private:
    //==============================================================================
    juce::TreeView tree;
    juce::TextButton undoButton, redoButton;    // [1]
    std::unique_ptr<ValueTreeItem> rootItem;
    juce::UndoManager undoManager;              // [1]

    void timerCallback() override               // [2]
    {
        undoManager.beginNewTransaction();      // [4]
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
