export default function Login() {
  return (
    <div className="min-h-[60vh] flex items-center justify-center bg-soft-grey">
      <div className="w-full max-w-md px-6">
        <div className="navy p-6 rounded-xl shadow-lg">
          <h2 className="text-2xl font-bold text-white mb-6 text-center">Welcome Back</h2>
          <form className="space-y-4" onSubmit={(e) => e.preventDefault()}>
            <div>
              <label className="block text-sm text-teal-100 mb-1">Email</label>
              <input
                type="email"
                className="w-full px-3 py-2 rounded bg-teal-50 border border-teal-200 text-navy focus:outline-none"
                placeholder="you@example.com"
              />
            </div>
            <div>
              <label className="block text-sm text-teal-100 mb-1">Password</label>
              <input
                type="password"
                className="w-full px-3 py-2 rounded bg-teal-50 border border-teal-200 text-navy focus:outline-none"
                placeholder="••••••••"
              />
            </div>
            <button
              type="submit"
              className="w-full teal bg-teal text-white py-2 rounded-lg font-medium hover:bg-teal-900 transition"
            >
              Sign in
            </button>
          </form>
          <p className="mt-4 text-center text-teal-100 text-sm">
            New to MediFind?{' '}
            <a href="/register" className="text-white underline">Create an account</a>
          </p>
        </div>
      </div>
    </div>
  );
}
