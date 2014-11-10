#ifndef SCOPE_PREVIEW_H_
#define SCOPE_PREVIEW_H_

#include <unity/scopes/PreviewQueryBase.h>

namespace unity {
namespace scopes {
class Result;
}
}

namespace scope {

/**
 * Represents an individual preview request.
 *
 * Each time a result is previewed in the UI a new Preview
 * object is created.
 */
class Preview: public unity::scopes::PreviewQueryBase {
public:
    Preview(const unity::scopes::Result &result,
            const unity::scopes::ActionMetadata &metadata);

    ~Preview() = default;

    void cancelled() override;

    /**
     * Populates the reply object with preview information.
     */
    void run(unity::scopes::PreviewReplyProxy const& reply) override;
};

}

#endif // SCOPE_PREVIEW_H_

