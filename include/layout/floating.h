/* floating layout — all windows are floating; no tiling arrangement needed */
void floating_layout(Monitor *m) {
	/* Nothing to arrange: in floating mode all clients are floating.
	 * The transition logic (converting tiled→floating and back) lives in
	 * setlayout / bind_define.h so that it runs exactly once when the
	 * layout changes, not on every arrange() call. */
	(void)m;
}
