#include <aether/domain/minimize.h>
#include <aether/domain/client.h>
#include <aether/domain/snap.h>

static Client *clientroot(Client *c);
static int clienttreecontains(Client *root, Client *needle);
static void setminimizedtree(Client *root, int minimized, int *need_refocus);

static Client *
clientroot(Client *c)
{
	Client *parent;

	while ((parent = client_get_parent(c)))
		c = parent;
	return c;
}

static int
clienttreecontains(Client *root, Client *needle)
{
	Client *c;

	if (root == needle)
		return 1;

	wl_list_for_each(c, &clients, link) {
		if (client_get_parent(c) == root && clienttreecontains(c, needle))
			return 1;
	}

	return 0;
}

static void
setminimizedtree(Client *root, int minimized, int *need_refocus)
{
	Client *c;

	if (root->isminimized != minimized) {
		root->isminimized = minimized;
		client_set_minimized(root, minimized);
		if (root->toplevel_handle)
			wlr_foreign_toplevel_handle_v1_set_minimized(root->toplevel_handle, minimized);
		if (root == grabc && minimized) {
			cursor_mode = CurNormal;
			grabc = NULL;
			snapclienthide();
			wlr_cursor_set_xcursor(cursor, cursor_mgr, "default");
		}
		if (root->scene) {
			wlr_scene_node_set_enabled(&root->scene->node,
					client_is_visible_on(root, root->mon));
			client_set_suspended(root, !client_is_visible_on(root, root->mon));
		}
	}

	if (need_refocus && seat->keyboard_state.focused_surface
			&& client_surface(root) == seat->keyboard_state.focused_surface
			&& minimized) {
		*need_refocus = 1;
	}

	wl_list_for_each(c, &clients, link) {
		if (client_get_parent(c) == root)
			setminimizedtree(c, minimized, need_refocus);
	}
}

void
minimizenotify(struct wl_listener *listener, void *data)
{
	Client *c = wl_container_of(listener, c, minimize);
	int minimized;

#ifdef XWAYLAND
	if (client_is_x11(c))
		minimized = ((struct wlr_xwayland_minimize_event *)data)->minimize;
	else
#endif
		minimized = client_wants_minimized(c);

	setminimized(c, minimized);
}

void
restoreminimized(const Arg *arg)
{
	Client *c;

	if (!selmon)
		return;

	wl_list_for_each(c, &fstack, flink) {
		if (c->isminimized && c->mon == selmon
				&& (c->tags & selmon->tagset[selmon->seltags])) {
			setminimized(c, 0);
			return;
		}
	}
}

void
setminimized(Client *c, int minimized)
{
	Client *root, *focus = NULL;
	int need_refocus = 0;

	if (!c || client_is_unmanaged(c) || !c->mon || !client_surface(c)->mapped)
		return;

	root = clientroot(c);
	minimized = !!minimized;

	if (root->isminimized == minimized)
		return;

	if (seat->keyboard_state.focused_surface) {
		wl_list_for_each(c, &clients, link) {
			if (client_surface(c) == seat->keyboard_state.focused_surface) {
				focus = c;
				break;
			}
		}
	}

	setminimizedtree(root, minimized, &need_refocus);
	arrange(root->mon);

	if (minimized) {
		if (focus && clienttreecontains(root, focus))
			focusclient(focustop(selmon), 1);
	} else if (client_is_visible_on(root, root->mon)) {
		focusclient(root, 1);
	}

	printstatus();
}
