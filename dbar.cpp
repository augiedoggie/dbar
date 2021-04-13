// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include <Deskbar.h>
#include <Roster.h>

#include <iostream>

using namespace std;


const char* kDeskbarMimeType = "application/x-vnd.Be-TSKB";


void scan_deskbar()
{
	cout << "Scanning Deskbar items..." << endl;

	BDeskbar deskbar;
	uint32 itemsFound = 0;

	for (uint32 id = 0; itemsFound < deskbar.CountItems(); id++) {
		if (!deskbar.HasItem(id))
			continue;

		const char* name;
		deskbar.GetItemInfo(id, &name);
		cout << "ID: " << id << " Name: " << name << endl;
		itemsFound++;
	}
}


void delete_item(const char* nameOrId)
{
	cout << "Removing Deskbar item: '" << nameOrId << "'" << endl;

	BDeskbar deskbar;

	// If the name only contains numbers it is probably an ID.
	if (strspn(nameOrId, "0123456789") < strlen(nameOrId)) {
		if (!deskbar.HasItem(nameOrId)) {
			cout << "Name '" << nameOrId << "' not found in Deskbar!" << endl;
			return;
		}

		if (deskbar.RemoveItem(nameOrId) != B_OK)
			cout << "Error removing item!" << endl;
	} else {
		int32 id = atoi(nameOrId);

		if (!deskbar.HasItem(id)) {
			cout << "ID '" << id << "' not found in Deskbar!" << endl;
			return;
		}

		if (deskbar.RemoveItem(id) != B_OK)
			cout << "Error removing item!" << endl;
	}
}


void add_item(const char* path)
{
	cout << "Adding Deskbar item: '" << path << "'" << endl;

	entry_ref ref;
	status_t status = get_ref_for_path(path, &ref);

	if (status != B_OK) {
		cout << "Error getting entry_ref: " << strerror(status) << endl;
		return;
	}

	BEntry entry(&ref, true);

	if (entry.InitCheck() != B_OK) {
		cout << "Error setting BEntry: Invalid path specified" << endl;
		return;
	}

	if (!entry.Exists() || !entry.IsFile()) {
		cout << "Error adding item: Path doesn't exist or isn't a file" << endl;
		return;
	}

	if (BDeskbar().AddItem(&ref) != B_OK) {
		cout << "Error adding item to Deskbar!" << endl;
		return;
	}
}


status_t quit_deskbar()
{
	cout << "Quitting Deskbar..." << endl;

	status_t status;
	BMessenger messenger(kDeskbarMimeType, -1, &status);

	// return B_OK because Deskbar isn't running
	if (status != B_OK) {
		cout << "Couldn't get Deskbar messenger!" << endl;
		return B_OK;
	}

	if (messenger.SendMessage(B_QUIT_REQUESTED) != B_OK) {
		cout << "Error sending quit message to Deskbar!" << endl;
		return B_ERROR;
	}

	return B_OK;
}


status_t kill_deskbar()
{
	cout << "Killing Deskbar..." << endl;

	app_info info;
	BRoster roster;

	if (roster.GetAppInfo(kDeskbarMimeType, &info) != B_OK) {
		cout << "Couldn't find Deskbar team." << endl;
		return B_ERROR;
	}

	cout << "Killing team:" << info.team << endl;

	if (kill_team(info.team) != B_OK) {
		cout << "Error sending kill signal to Deskbar!" << endl;
		return B_ERROR;
	}

	return B_OK;
}


void restart_deskbar()
{
	if (quit_deskbar() != B_OK) {
		if (kill_deskbar() != B_OK)
			return;
	}

	BRoster roster;
	uint32 loops = 0;

	while (roster.IsRunning(kDeskbarMimeType)) {
		loops++;

		if (loops > 1000) {
			cout << "Timeout waiting for Deskbar to quit!" << endl;
			return;
		}

		snooze(1000);
	}

	cout << "Restarting Deskbar..." << endl;

	roster.Launch(kDeskbarMimeType);
}


void print_usage(const char* progname) {
	cout << "Usage: " << progname << " [-s] [-r] [-k] [-d id/name] [-a /path/to/replicant]" << endl;
	cout << "  -s: Scan deskbar items and print ID/Name." << endl;
	cout << "  -r: Restart deskbar." << endl;
	cout << "  -q: Quit deskbar." << endl;
	cout << "  -k: Kill deskbar." << endl;
	cout << "  -d: Delete deskbar item by either ID or Name." << endl;
	cout << "  -a: Add deskbar item using path to the replicant application." << endl;
}


int main(int argc, char** argv) {

	if (argc == 1) {
		print_usage(argv[0]);
		return 1;
	}

	bool optFound = false;
	int c;

	while ((c = getopt(argc, argv, "hsqkrd:a:")) != -1) {
		optFound = true;

		switch (c) {
			case 's':
				scan_deskbar();
				break;
			case 'r':
				restart_deskbar();
				break;
			case 'q':
				quit_deskbar();
				break;
			case 'k':
				kill_deskbar();
				break;
			case 'd':
				delete_item(optarg);
				break;
			case 'a':
				add_item(optarg);
				break;
			case 'h':
			default:
				print_usage(argv[0]);
				return 1;
		}
	}

	if (!optFound) {
		print_usage(argv[0]);
		return 1;
	}

	return 0;
}
