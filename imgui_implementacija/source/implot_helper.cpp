#include "implot_helper.h"

template <typename TGetterX, typename TGetterY>
struct GetterXY {
    GetterXY(TGetterX x, TGetterY y, int count) : GetterX(x), GetterY(y), Count(count) { }
    template <typename I> ImPlotPoint operator()(I idx) const {
        return ImPlotPoint(GetterX(idx), GetterY(idx));
    }
    const TGetterX GetterX;
    const TGetterY GetterY;
    const int Count;
};

struct GetterLin {
    GetterLin(double m, double b) : M(m), B(b) { }
    template <typename I> double operator()(I idx) const {
        return M * idx + B;
    }
    const double M;
    const double B;
};

struct GetterRef {
    GetterRef(double ref) : Ref(ref) { }
    template <typename I> double operator()(I) const { return Ref; }
    const double Ref;
};

template <typename T>
T IndexData(const T* data, int idx, int count, int offset, int stride) {
    const int s = ((offset == 0) << 0) | ((stride == sizeof(T)) << 1);
    switch (s) {
    case 3: return data[idx];
    case 2: return data[(offset + idx) % count];
    case 1: return *(const T*)(const void*)((const unsigned char*)data + (size_t)((idx)) * stride);
    case 0: return *(const T*)(const void*)((const unsigned char*)data + (size_t)((offset + idx) % count) * stride);
    default: return T(0);
    }
}

template <typename T>
struct GetterIdx {
    GetterIdx(const T* data, int count, int offset = 0, int stride = sizeof(T)) :
        Data(data),
        Count(count),
        Offset(count ? ImPosMod(offset, count) : 0),
        Stride(stride)
    { }
    template <typename I> double operator()(I idx) const {
        return (double)IndexData(Data, idx, Count, Offset, Stride);
    }
    const T* Data;
    int Count;
    int Offset;
    int Stride;
};

void MyImPlot::PlotBarGroups(const GraphData& gd)
{
    PlotBarGroups(&gd.item_labels[0], &gd.plot_data[0], gd.items, gd.groups, gd.size, &gd.colors[0]);
}

void MyImPlot::PlotBarGroups(const char* const label_ids[], const int* values, int items, int groups, double width, const glm::vec3* item_colors)
{
    const double subwidth = width / items;
    for (int i = 0; i < items; ++i) {
        const double subshift = (i + 0.5) * subwidth - width / 2;
        const glm::vec3& color = item_colors[i];
        PlotBars(label_ids[i], &values[i * groups], groups, ImGui::GetColorU32({color.r, color.g, color.b, 1}), subwidth, subshift);
    }
}

void MyImPlot::PlotBars(const char* label_id, const int* values, int count, ImU32 color, double width, double x0, int offset, int stride)
{
    GetterXY<GetterLin, GetterIdx<int>> getter1(GetterLin(1.0, x0), GetterIdx<int>(values, count, offset, stride), count);
    GetterXY<GetterLin, GetterRef> getter2(GetterLin(1.0, x0), GetterRef(0), count);

    if (ImPlot::BeginItem(label_id, ImPlotCol_Fill)) {
        const double half_width = width / 2;
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < getter1.Count; ++i) {
                ImPlotPoint p1 = getter1(i);
                ImPlotPoint p2 = getter2(i);
                ImPlot::FitPoint(ImPlotPoint(p1.x - half_width, p1.y));
                ImPlot::FitPoint(ImPlotPoint(p2.x + half_width, p2.y));
            }
        }

        const ImPlotNextItemData& s = ImPlot::GetItemData();
        ImDrawList& DrawList = *ImPlot::GetPlotDrawList();

        ImPlot::GetCurrentItem()->Color = color;            //set legend color
        ImU32 col_line = ImGui::GetColorU32({0, 0, 0 ,0});  //set line color
        ImU32 col_fill = color;                             //set fill color
        bool  rend_line = false;

        for (int i = 0; i < getter1.Count; ++i) {
            ImPlotPoint p1 = getter1(i);
            ImPlotPoint p2 = getter2(i);
            if (p1.y == p2.y)
                continue;
            ImVec2 a = ImPlot::PlotToPixels(p1.x - half_width, p1.y, IMPLOT_AUTO, IMPLOT_AUTO);
            ImVec2 b = ImPlot::PlotToPixels(p2.x + half_width, p2.y, IMPLOT_AUTO, IMPLOT_AUTO);
            float width_px = ImAbs(a.x - b.x);
            if (width_px < 1.0f) {
                a.x += a.x > b.x ? (1 - width_px) / 2 : (width_px - 1) / 2;
                b.x += b.x > a.x ? (1 - width_px) / 2 : (width_px - 1) / 2;
            }

            if (s.RenderFill)
                DrawList.AddRectFilled(a, b, col_fill);
            if (rend_line)
                DrawList.AddRect(a, b, col_line, 0, ImDrawFlags_RoundCornersAll, s.LineWeight);
        }
        ImPlot::EndItem();
    }
}