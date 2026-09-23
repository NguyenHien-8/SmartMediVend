#pragma once

#include <array>
#include <cctype>
#include <string>
#include <string_view>
#include <utility>

namespace smv {
// The submitted table is a TEST FIXTURE, not medical advice or a prescription.
struct TestItem {
    std::string_view keyword;
    unsigned relay;
    std::string_view sku;
};
inline constexpr std::array<TestItem, 16> kTestItems{{
    {"đau bụng", 1, "Betahistin"}, {"nhức đầu", 2, "Paracetamol"},
    {"giảm đau", 3, "Paracetamol"}, {"hạ huyết áp", 4, "Captopril"},
    {"đau bao tử", 5, "Trimafort"}, {"ho đờm", 6, "Acetylcystin"},
    {"ho khan", 7, "Eugica"}, {"cảm", 8, "Dextromethorphan"},
    {"đau cơ", 9, "Eperison"}, {"sổ mũi", 10, "Fexofenadine"},
    {"tiêu chảy", 11, "Simethicone"}, {"buồn nôn", 12, "Domperion"},
    {"đau bụng kinh", 13, "dolfenal"}, {"thuốc chống dị ứng", 14, "histamin"},
    {"thuốc giãn phế", 15, "Salbutamol"}, {"thuốc hạ sốt", 16, "Panadol"},
}};

struct Match {
    const TestItem* item = nullptr;
    bool ambiguous = false;
};

inline std::string NormalizeTranscript(std::string_view text) {
    // UTF-8 Vietnamese uppercase letters must be folded explicitly; std::tolower
    // only lowercases the ASCII part of a UTF-8 byte sequence.
    static constexpr std::pair<std::string_view, std::string_view> folds[] = {
        {"À", "à"},
        {"Á", "á"},
        {"Â", "â"},
        {"Ã", "ã"},
        {"Ä", "ä"},
        {"Å", "å"},
        {"Æ", "æ"},
        {"È", "è"},
        {"É", "é"},
        {"Ê", "ê"},
        {"Ë", "ë"},
        {"Ì", "ì"},
        {"Í", "í"},
        {"Î", "î"},
        {"Ï", "ï"},
        {"Ð", "ð"},
        {"Ò", "ò"},
        {"Ó", "ó"},
        {"Ô", "ô"},
        {"Õ", "õ"},
        {"Ö", "ö"},
        {"Ø", "ø"},
        {"Ù", "ù"},
        {"Ú", "ú"},
        {"Û", "û"},
        {"Ü", "ü"},
        {"Ý", "ý"},
        {"Ā", "ā"},
        {"Ă", "ă"},
        {"Ą", "ą"},
        {"Đ", "đ"},
        {"Ē", "ē"},
        {"Ĕ", "ĕ"},
        {"Ė", "ė"},
        {"Ę", "ę"},
        {"Ě", "ě"},
        {"Ĩ", "ĩ"},
        {"Ī", "ī"},
        {"Ĭ", "ĭ"},
        {"Į", "į"},
        {"Ŋ", "ŋ"},
        {"Ō", "ō"},
        {"Ŏ", "ŏ"},
        {"Ő", "ő"},
        {"Ũ", "ũ"},
        {"Ū", "ū"},
        {"Ŭ", "ŭ"},
        {"Ů", "ů"},
        {"Ű", "ű"},
        {"Ų", "ų"},
        {"Ŷ", "ŷ"},
        {"Ÿ", "ÿ"},
        {"Ɔ", "ɔ"},
        {"Ɖ", "ɖ"},
        {"Ɛ", "ɛ"},
        {"Ɩ", "ɩ"},
        {"Ɨ", "ɨ"},
        {"Ɵ", "ɵ"},
        {"Ơ", "ơ"},
        {"Ƣ", "ƣ"},
        {"Ʀ", "ʀ"},
        {"Ʃ", "ʃ"},
        {"Ư", "ư"},
        {"Ʊ", "ʊ"},
        {"Ƴ", "ƴ"},
        {"Ʒ", "ʒ"},
        {"Ƹ", "ƹ"},
        {"Ǎ", "ǎ"},
        {"Ǐ", "ǐ"},
        {"Ǒ", "ǒ"},
        {"Ǔ", "ǔ"},
        {"Ǖ", "ǖ"},
        {"Ǘ", "ǘ"},
        {"Ǚ", "ǚ"},
        {"Ǜ", "ǜ"},
        {"Ǟ", "ǟ"},
        {"Ǡ", "ǡ"},
        {"Ǣ", "ǣ"},
        {"Ǫ", "ǫ"},
        {"Ǭ", "ǭ"},
        {"Ǯ", "ǯ"},
        {"Ǻ", "ǻ"},
        {"Ǽ", "ǽ"},
        {"Ǿ", "ǿ"},
        {"Ȁ", "ȁ"},
        {"Ȃ", "ȃ"},
        {"Ȅ", "ȅ"},
        {"Ȇ", "ȇ"},
        {"Ȉ", "ȉ"},
        {"Ȋ", "ȋ"},
        {"Ȍ", "ȍ"},
        {"Ȏ", "ȏ"},
        {"Ȕ", "ȕ"},
        {"Ȗ", "ȗ"},
        {"Ȝ", "ȝ"},
        {"Ȣ", "ȣ"},
        {"Ȧ", "ȧ"},
        {"Ȩ", "ȩ"},
        {"Ȫ", "ȫ"},
        {"Ȭ", "ȭ"},
        {"Ȯ", "ȯ"},
        {"Ȱ", "ȱ"},
        {"Ȳ", "ȳ"},
        {"Ⱥ", "ⱥ"},
        {"Ʉ", "ʉ"},
        {"Ɇ", "ɇ"},
        {"Ɏ", "ɏ"},
        {"Ḁ", "ḁ"},
        {"Ḕ", "ḕ"},
        {"Ḗ", "ḗ"},
        {"Ḙ", "ḙ"},
        {"Ḛ", "ḛ"},
        {"Ḝ", "ḝ"},
        {"Ḭ", "ḭ"},
        {"Ḯ", "ḯ"},
        {"Ṍ", "ṍ"},
        {"Ṏ", "ṏ"},
        {"Ṑ", "ṑ"},
        {"Ṓ", "ṓ"},
        {"Ṳ", "ṳ"},
        {"Ṵ", "ṵ"},
        {"Ṷ", "ṷ"},
        {"Ṹ", "ṹ"},
        {"Ṻ", "ṻ"},
        {"Ẏ", "ẏ"},
        {"Ạ", "ạ"},
        {"Ả", "ả"},
        {"Ấ", "ấ"},
        {"Ầ", "ầ"},
        {"Ẩ", "ẩ"},
        {"Ẫ", "ẫ"},
        {"Ậ", "ậ"},
        {"Ắ", "ắ"},
        {"Ằ", "ằ"},
        {"Ẳ", "ẳ"},
        {"Ẵ", "ẵ"},
        {"Ặ", "ặ"},
        {"Ẹ", "ẹ"},
        {"Ẻ", "ẻ"},
        {"Ẽ", "ẽ"},
        {"Ế", "ế"},
        {"Ề", "ề"},
        {"Ể", "ể"},
        {"Ễ", "ễ"},
        {"Ệ", "ệ"},
        {"Ỉ", "ỉ"},
        {"Ị", "ị"},
        {"Ọ", "ọ"},
        {"Ỏ", "ỏ"},
        {"Ố", "ố"},
        {"Ồ", "ồ"},
        {"Ổ", "ổ"},
        {"Ỗ", "ỗ"},
        {"Ộ", "ộ"},
        {"Ớ", "ớ"},
        {"Ờ", "ờ"},
        {"Ở", "ở"},
        {"Ỡ", "ỡ"},
        {"Ợ", "ợ"},
        {"Ụ", "ụ"},
        {"Ủ", "ủ"},
        {"Ứ", "ứ"},
        {"Ừ", "ừ"},
        {"Ử", "ử"},
        {"Ữ", "ữ"},
        {"Ự", "ự"},
        {"Ỳ", "ỳ"},
        {"Ỵ", "ỵ"},
        {"Ỷ", "ỷ"},
        {"Ỹ", "ỹ"},
    };
    std::string folded(text);
    for (const auto& fold : folds) {
        size_t i = 0;
        while ((i = folded.find(fold.first, i)) != std::string::npos) {
            folded.replace(i, fold.first.size(), fold.second);
            i += fold.second.size();
        }
    }
    std::string out;
    out.reserve(folded.size());
    bool previous_space = true;
    for (unsigned char c : folded) {
        if (c < 128 && (std::isspace(c) || std::ispunct(c))) {
            if (!previous_space) out.push_back(' ');
            previous_space = true;
        } else {
            out.push_back(static_cast<char>(c < 128 ? std::tolower(c) : c));
            previous_space = false;
        }
    }
    if (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

inline bool HasPhrase(std::string_view input, std::string_view phrase) {
    size_t pos = 0;
    while ((pos = input.find(phrase, pos)) != std::string_view::npos) {
        const size_t end = pos + phrase.size();
        if ((pos == 0 || input[pos - 1] == ' ') &&
            (end == input.size() || input[end] == ' ')) return true;
        ++pos;
    }
    return false;
}

inline Match MatchTestKeyword(std::string_view transcript) {
    // Reject negation, questions and unclear multi-symptom utterances rather than
    // triggering anything from a substring of a conversational sentence.
    const auto normalized = NormalizeTranscript(transcript);
    if (normalized.empty() || normalized.size() > 512 ||
        HasPhrase(normalized, "không") || HasPhrase(normalized, "chẳng") ||
        HasPhrase(normalized, "chưa") || HasPhrase(normalized, "đừng") ||
        HasPhrase(normalized, "hỏi") || HasPhrase(normalized, "cảm ơn") ||
        HasPhrase(normalized, "cảm thấy") || HasPhrase(normalized, "cảm giác") ||
        HasPhrase(normalized, "tại sao") ||
        HasPhrase(normalized, "thế nào") || HasPhrase(normalized, "bao nhiêu") ||
        transcript.find('?') != std::string_view::npos) return {};

    const TestItem* found = nullptr;
    for (const auto& item : kTestItems) {
        if (!HasPhrase(normalized, item.keyword)) continue;
        if (found) {
            // Prefer the longer, more specific phrase only when one entire
            // keyword contains the other (e.g. đau bụng kinh vs đau bụng).
            if (HasPhrase(item.keyword, found->keyword)) {
                found = &item;
                continue;
            }
            if (HasPhrase(found->keyword, item.keyword)) continue;
            return {nullptr, true};
        }
        found = &item;
    }
    return {found, false};
}
}  // namespace smv
