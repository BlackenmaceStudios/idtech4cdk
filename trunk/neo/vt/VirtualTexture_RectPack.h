// VirtualTexture_RectPack.h
//

// http://www.gamedev.net/topic/392413-rectangle-packing/

#ifndef BINPACKER_H
#define BINPACKER_H

#undef strcmp
#undef strncmp
#define FLT_EPSILON 1.19209290E-07F

#include <float.h>
#include <vector>
#include <math.h>

#include <limits>




class BinPacker
{
public:

    // The input and output are in terms of vectors of ints to avoid
    // dependencies (although I suppose a public member struct could have been
    // used). The parameters are:
    
    // rects : An array containing the width and height of each input rect in
    // sequence, i.e. [w0][h0][w1][h1][w2][h2]... The IDs for the rects are
    // derived from the order in which they appear in the array.
    
    // packs : After packing, the outer array contains the packs (therefore
    // the number of packs is packs.size()). Each inner array contains a
    // sequence of sets of 4 ints. Each set represents a rectangle in the
    // pack. The elements in the set are 1) the rect ID, 2) the x position
    // of the rect with respect to the pack, 3) the y position of the rect
    // with respect to the pack, and 4) whether the rect was rotated (1) or
    // not (0). The widths and heights of the rects are not included, as it's
    // assumed they are stored on the caller's side (they were after all the
    // input to the function).
    
    // allowRotation : when true (the default value), the packer is allowed
    // the option of rotating the rects in the process of trying to fit them
    // into the current working area.

    void Pack(
        const std::vector< int >&          rects,
        std::vector<  std::vector< int > >& packs,
        int                              packSize,
        bool                             allowRotation = true
    );

private:

    struct Rect
    {
        Rect(int size)
            : x(0), y(0), w(size), h(size), ID(-1), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }

        Rect(int x, int y, int w, int h, int ID = 1)
            : x(x), y(y), w(w), h(h), ID(ID), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }
        
        int GetArea() const {
            return w * h;
        }
        
        void Rotate() {
            std::swap(w, h);
            rotated = !rotated;
        }
        
        bool operator<(const Rect& rect) const {
            return GetArea() < rect.GetArea();
        }

        int  x;
        int  y;
        int  w;
        int  h;
        int  ID;
        int  children[2];
        bool rotated;
        bool packed;
    };

    void Clear();
    void Fill(int pack, bool allowRotation);
    void Split(int pack, int rect);
    bool Fits(Rect& rect1, const Rect& rect2, bool allowRotation);
    void AddPackToArray(int pack, std::vector<int>& array) const;
    
    bool RectIsValid(int i) const;
    bool PackIsValid(int i) const;
    
    int               m_packSize;
    int               m_numPacked;
    std::vector<Rect>	 m_rects;
   std::vector<Rect>	 m_packs;
    std::vector<int>      m_roots;
};

#endif // #ifndef BINPACKER_H


#include <algorithm>

// ---------------------------------------------------------------------------
void BinPacker::Pack(
    const  std::vector< int >&          rects,
     std::vector<  std::vector< int > >& packs,
    int                              packSize,
    bool                             allowRotation)
{
    assert(!(rects.size() % 2));
    
    Clear();
    
    m_packSize = packSize;

    // Add rects to member array, and check to make sure none is too big
    for (size_t i = 0; i < rects.size(); i += 2) {
        if (rects[i] > m_packSize || rects[i + 1] > m_packSize) {
            assert(!"All rect dimensions must be <= the pack size");
        }
        m_rects.push_back(Rect(0, 0, rects[i], rects[i + 1], i >> 1));
    }
    
    // Sort from greatest to least area
    std::sort(m_rects.rbegin(), m_rects.rend());

    // Pack
    while (m_numPacked < (int)m_rects.size()) {
        int i = m_packs.size();
        m_packs.push_back(Rect(m_packSize));
        m_roots.push_back(i);
        Fill(i, allowRotation);
    }
    
    // Write out
    packs.resize(m_roots.size());
    for (size_t i = 0; i < m_roots.size(); ++i) {
        packs[i].clear();
        AddPackToArray(m_roots[i], packs[i]);
    }

    // Check and make sure all rects were packed
    for (size_t i = 0; i < m_rects.size(); ++i) {
        if (!m_rects[i].packed) {
            assert(!"Not all rects were packed");
        }
    }
}
// ---------------------------------------------------------------------------
void BinPacker::Clear()
{
    m_packSize = 0;
    m_numPacked = 0;
    m_rects.clear();
    m_packs.clear();
    m_roots.clear();
}
// ---------------------------------------------------------------------------
void BinPacker::Fill(int pack, bool allowRotation)
{
    assert(PackIsValid(pack));

    int i = pack;
    
    // For each rect
    for (size_t j = 0; j < m_rects.size(); ++j) {
        // If it's not already packed
        if (!m_rects[j].packed) {
            // If it fits in the current working area
            if (Fits(m_rects[j], m_packs[i], allowRotation)) {
                // Store in lower-left of working area, split, and recurse
                ++m_numPacked;
                Split(i, j);
                Fill(m_packs[i].children[0], allowRotation);
                Fill(m_packs[i].children[1], allowRotation);
                return;
            }
        }
    }
}
// ---------------------------------------------------------------------------
void BinPacker::Split(int pack, int rect)
{
    assert(PackIsValid(pack));
    assert(RectIsValid(rect));
    
    int i = pack;
    int j = rect;

    // Split the working area either horizontally or vertically with respect
    // to the rect we're storing, such that we get the largest possible child
    // area.

    Rect left = m_packs[i];
    Rect right = m_packs[i];
    Rect bottom = m_packs[i];
    Rect top = m_packs[i];

    left.y += m_rects[j].h/ 2;
    left.w = m_rects[j].w / 2;
    left.h -= m_rects[j].h / 2;
    right.x += m_rects[j].w/ 2;
    right.w -= m_rects[j].w / 2;
    
    bottom.x += m_rects[j].w/ 2;
    bottom.h = m_rects[j].h / 2;
    bottom.w -= m_rects[j].w / 2;
    top.y += m_rects[j].h/ 2;
    top.h -= m_rects[j].h / 2;
    
    int maxLeftRightArea = left.GetArea();
    if (right.GetArea() > maxLeftRightArea) {
        maxLeftRightArea = right.GetArea();
    }
    
    int maxBottomTopArea = bottom.GetArea();
    if (top.GetArea() > maxBottomTopArea) {
        maxBottomTopArea = top.GetArea();
    }

    if (maxLeftRightArea > maxBottomTopArea) {
        if (left.GetArea() > right.GetArea()) {
            m_packs.push_back(left);
            m_packs.push_back(right);
        } else {
            m_packs.push_back(right);
            m_packs.push_back(left);
        }
    } else {
        if (bottom.GetArea() > top.GetArea()) {
            m_packs.push_back(bottom);
            m_packs.push_back(top);
        } else {
            m_packs.push_back(top);
            m_packs.push_back(bottom);
        }
    }
    
    // This pack area now represents the rect we've just stored, so save the
    // relevant info to it, and assign children.
    m_packs[i].w = m_rects[j].w;
    m_packs[i].h = m_rects[j].h;
    m_packs[i].ID = m_rects[j].ID;
    m_packs[i].rotated = m_rects[j].rotated;
    m_packs[i].children[0] = m_packs.size() - 1;
    m_packs[i].children[1] = m_packs.size() - 2;
    
    // Done with the rect
    m_rects[j].packed = true;
}
// ---------------------------------------------------------------------------
bool BinPacker::Fits(Rect& rect1, const Rect& rect2, bool allowRotation)
{
    // Check to see if rect1 fits in rect2, and rotate rect1 if that will
    // enable it to fit.

    if (rect1.w <= rect2.w && rect1.h <= rect2.h) {
        return true;
    } else if (allowRotation && rect1.h <= rect2.w && rect1.w <= rect2.h) {
        rect1.Rotate();
        return true;
    } else {
        return false;
    }
}
// ---------------------------------------------------------------------------
void BinPacker::AddPackToArray(int pack, std::vector<int>& array) const
{
    assert(PackIsValid(pack));
    
    int i = pack;
    if (m_packs[i].ID != -1) {
        array.push_back(m_packs[i].ID);
        array.push_back(m_packs[i].x);
        array.push_back(m_packs[i].y);
        array.push_back(m_packs[i].rotated);
        
        if (m_packs[i].children[0] != -1) {
            AddPackToArray(m_packs[i].children[0], array);
        }
        if (m_packs[i].children[1] != -1) {
            AddPackToArray(m_packs[i].children[1], array);
        }
    }
}
// ---------------------------------------------------------------------------
bool BinPacker::RectIsValid(int i) const
{
    return i >= 0 && i < (int)m_rects.size();
}
// ---------------------------------------------------------------------------
bool BinPacker::PackIsValid(int i) const
{
    return i >= 0 && i < (int)m_packs.size();
}
// ---------------------------------------------------------------------------
