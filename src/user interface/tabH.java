import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class tabH implements ActionListener {

    private String tabName;

    public tabH(String tabName) {
        this.tabName = tabName;
    }

    public String getTabName() {
        return tabName;
    }

    public void actionPerformed(ActionEvent evt) {

        int index = test.tab.indexOfTab(getTabName());
        if (index >= 0) {

            test.tab.removeTabAt(index);
            // It would probably be worthwhile getting the source
            // casting it back to a JButton and removing
            // the action handler reference ;)

        }

    }

}