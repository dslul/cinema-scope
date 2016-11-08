#ifndef SCOPE_H_
#define SCOPE_H_

#include <client.h>

#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/ReplyProxyFwd.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/PreviewQueryBase.h>

/**
 * Defines the lifecycle of scope plugin, and acts as a factory
 * for Query and Preview objects.
 *
 * Note that the #preview and #search methods are each called on
 * different threads, so some form of interlocking is required
 * if shared data structures are used.
 */
class Scope: public unity::scopes::ScopeBase {
public:
    /**
     * Called once at startup
     */
    void start(std::string const&) override;

    /**
     * Called at shutdown
     */
    void stop() override;

    /**
     * Called each time a new preview is requested
     */
    unity::scopes::PreviewQueryBase::UPtr preview(const unity::scopes::Result&,
                                                  const unity::scopes::ActionMetadata&) override;

    /**
     * Called each time a new query is requested
     */
    unity::scopes::SearchQueryBase::UPtr search(
            unity::scopes::CannedQuery const& q,
            unity::scopes::SearchMetadata const&) override;

protected:
    Config::Ptr config_;
};

#endif // SCOPE_H_

