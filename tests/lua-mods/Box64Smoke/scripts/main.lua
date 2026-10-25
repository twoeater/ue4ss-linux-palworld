-- Enable temporarily to verify reflection, the game-thread hook, async dispatch,
-- and Lua error handling in the actual emulated server.
print("[Box64Smoke] LUA_STARTED\n")

local ok, message = pcall(function()
    error("expected smoke-test error")
end)
assert(not ok and message:find("expected smoke-test error", 1, true))
print("[Box64Smoke] LUA_ERROR_HANDLED\n")

ExecuteWithDelay(1000, function()
    print("[Box64Smoke] ASYNC_DISPATCH_OK\n")
    ExecuteInGameThread(function()
        local actorClass = StaticFindObject("/Script/Engine.Actor")
        assert(actorClass and actorClass:IsValid(), "Engine.Actor was not resolved")
        print("[Box64Smoke] GAME_THREAD_REFLECTION_OK " .. actorClass:GetFullName() .. "\n")
    end)
end)

ExecuteWithDelay(30000, function()
    ExecuteInGameThread(function()
        print("[Box64Smoke] GAME_THREAD_ALIVE_30S\n")
    end)
end)
