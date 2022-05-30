#include "implot_helper.h"
#include "implot.h"
#include "implot_internal.h"

void MyImPlot::PlotBarGroups(const GraphData& gd, float bar_width)
{
	std::vector<ImU32> colors;//quick-fix ->TODO: REPLACE glm::vec3 for colors in GraphData with ImU32
	for (const auto& c : gd.colors)
		colors.push_back(ImGui::GetColorU32({ c.r, c.g, c.b, 1 }));
    ImPlot::PlotBarGroupsCustomColors(&gd.item_labels[0], &gd.plot_data[0], &colors[0], gd.item_labels.size(), gd.group_labels.size(), bar_width);
}

void MyImPlot::PlotBars(const GraphData& gd, unsigned int index, float bar_width)
{
	unsigned int num_of_groups = gd.group_labels.size();
	const glm::vec3& color = gd.colors[index];
	ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
	ImPlot::PlotBarsCustomColor(gd.item_labels[index], &gd.plot_data[0 + index * num_of_groups], packed_color, num_of_groups, bar_width);
}


void MyImPlot::PlotLine(const GraphData& gd, unsigned int line_index)
{
	unsigned int num_of_groups = gd.group_labels.size();
	const glm::vec3& color = gd.colors[line_index];
	ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
	ImPlot::PlotLineCustomColor(gd.item_labels[line_index], &gd.positions[0], &gd.plot_data[0 + line_index * num_of_groups], packed_color, num_of_groups);
}

namespace MyImPlot {
    static inline void DefaultFormatter(double value, char* buff, int size, void* data) {
        char* fmt = (char*)data;
        ImFormatString(buff, size, fmt, value);
    }

    void RenderColorBar(const Gradient& gradient, unsigned int num_of_points, ImDrawList& DrawList, const ImRect& bounds, bool reversed, bool continuous) {
        const int n = continuous ? num_of_points - 1 : num_of_points;
        ImU32 col1, col2;

        const float step = bounds.GetHeight() / n;

        ImRect rect(bounds.Min.x, bounds.Min.y, bounds.Max.x, bounds.Min.y + step);

        for (int i = 0; i < n; ++i) {
            if (reversed) {
                col1 = continuous ? gradient.evaluate_packed(float(num_of_points - i - 1) / n) : gradient.evaluate_packed(float(num_of_points - i) / n);
                col2 = continuous ? gradient.evaluate_packed(float(num_of_points - i - 2) / n) : col1;
            }
            else {
                col1 = continuous ? gradient.evaluate_packed(float(i) / n) : gradient.evaluate_packed(float(i + 1) / n);
                col2 = continuous ? gradient.evaluate_packed(float(i + 1) / n) : col1;
            }
            DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col1, col2, col2);
            rect.TranslateY(step);
        }
    }
}



void MyImPlot::ColormapScale(const char* label, const Gradient& gradient, double scale_min, double scale_max, unsigned int num_of_color_points, const ImVec2& size, bool is_continuous)
{
    ImGuiContext& G = *GImGui;
    ImGuiWindow* Window = G.CurrentWindow;
    if (Window->SkipItems)
        return;

    const ImGuiID ID = Window->GetID(label);
    ImVec2 label_size(0, 0);
    label_size = ImGui::CalcTextSize(label, NULL, true);

    ImPlotContext& gp = *GImPlot;

    ImVec2 frame_size = ImGui::CalcItemSize(size, 0, gp.Style.PlotDefaultSize.y);
    if (frame_size.y < gp.Style.PlotMinSize.y && size.y < 0.0f)
        frame_size.y = gp.Style.PlotMinSize.y;

    ImPlotRange range(scale_min, scale_max);
    gp.CTicks.Reset();
    ImPlot::AddTicksDefault(range, frame_size.y, true, gp.CTicks, DefaultFormatter, (void*)"%g");

    const float txt_off = gp.Style.LabelPadding.x;
    const float pad_right = txt_off + gp.CTicks.MaxSize.x + (label_size.x > 0 ? txt_off + label_size.y : 0);
    float bar_w = 20;

    if (frame_size.x == 0)
        frame_size.x = bar_w + pad_right + 2 * gp.Style.PlotPadding.x;
    else {
        bar_w = frame_size.x - (pad_right + 2 * gp.Style.PlotPadding.x);
        if (bar_w < gp.Style.MajorTickLen.y)
            bar_w = gp.Style.MajorTickLen.y;
    }

    ImDrawList& DrawList = *Window->DrawList;
    ImRect bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ImGui::ItemSize(bb_frame);
    if (!ImGui::ItemAdd(bb_frame, ID, &bb_frame))
        return;

    ImGui::RenderFrame(bb_frame.Min, bb_frame.Max, ImPlot::GetStyleColorU32(ImPlotCol_FrameBg), true, G.Style.FrameRounding);
    ImRect bb_grad(bb_frame.Min + gp.Style.PlotPadding, bb_frame.Min + ImVec2(bar_w + gp.Style.PlotPadding.x, frame_size.y - gp.Style.PlotPadding.y));

    ImGui::PushClipRect(bb_frame.Min, bb_frame.Max, true);
    RenderColorBar(gradient, num_of_color_points, DrawList, bb_grad, true, is_continuous);
    
    const ImU32 col_tick = ImPlot::GetStyleColorU32(ImPlotCol_AxisText);
    const ImU32 col_text = ImGui::GetColorU32(ImGuiCol_Text);
    
    for (int i = 0; i < gp.CTicks.Size; ++i) {
        const float ypos = ImRemap((float)gp.CTicks.Ticks[i].PlotPos, (float)range.Max, (float)range.Min, bb_grad.Min.y, bb_grad.Max.y);
        const float tick_width = gp.CTicks.Ticks[i].Major ? gp.Style.MajorTickLen.y : gp.Style.MinorTickLen.y;
        const float tick_thick = gp.CTicks.Ticks[i].Major ? gp.Style.MajorTickSize.y : gp.Style.MinorTickSize.y;
        if (ypos < bb_grad.Max.y - 2 && ypos > bb_grad.Min.y + 2)
            DrawList.AddLine(ImVec2(bb_grad.Max.x - 1, ypos), ImVec2(bb_grad.Max.x - tick_width, ypos), col_tick, tick_thick);
        DrawList.AddText(ImVec2(bb_grad.Max.x - 1, ypos) + ImVec2(txt_off, -gp.CTicks.Ticks[i].LabelSize.y * 0.5f), col_text, gp.CTicks.GetText(i));
    }

    if (label_size.x > 0) {
        ImVec2 label_pos(bb_grad.Max.x - 1 + 2 * txt_off + gp.CTicks.MaxSize.x, bb_grad.GetCenter().y + label_size.x * 0.5f);
        const char* label_end = ImGui::FindRenderedTextEnd(label);
        ImPlot::AddTextVertical(&DrawList, label_pos, col_text, label, label_end);
    }

    DrawList.AddRect(bb_grad.Min, bb_grad.Max, ImPlot::GetStyleColorU32(ImPlotCol_PlotBorder));
    ImGui::PopClipRect();
}
